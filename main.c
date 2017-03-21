#include "mongoose.h"

struct align_default {
    uint8_t  uint8;
    uint32_t uint32;
} resp_a_def;

#pragma pack(1)
struct align_by1byte {
    uint8_t  uint8;
    uint32_t uint32;
} resp_a_by1;
#pragma pop()

static int glob_cnt = 1;

static sig_atomic_t s_signal_received = 0;
static const char *s_http_port = "8000";

static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}

static int is_websocket(const struct mg_connection *nc) {
    return nc->flags & MG_F_IS_WEBSOCKET;
}

enum resp_type { resp_alignedByDefault = 49, resp_alignedBy1Byte = 50 };

static void send_response(struct mg_connection *nc, enum resp_type rt) {
    void *data = NULL;
    size_t size = 0;

    switch (rt) {
        case resp_alignedByDefault:
            data = &resp_a_def;
            size = sizeof(resp_a_def);
            resp_a_def.uint8 = glob_cnt++;
            resp_a_def.uint32 = glob_cnt++;
            break;
        case resp_alignedBy1Byte:
            data = &resp_a_by1;
            size = sizeof(resp_a_by1);
            resp_a_by1.uint8 = glob_cnt++;
            resp_a_by1.uint32 = glob_cnt++;
            break;
    }
    mg_send_websocket_frame(nc, WEBSOCKET_OP_BINARY, data, size);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    switch (ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            // New websocket connection. Debug.
            printf("New websocket connection");
            break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
            struct websocket_message *wm = (struct websocket_message *) ev_data;
            enum resp_type rt = *wm->data;

            if (wm->size == 1 && (rt == resp_alignedByDefault || rt == resp_alignedBy1Byte)) {
                send_response(nc, rt);
            }
            break;
        }
        case MG_EV_CLOSE: {
            // Disconnect. Tell everybody.
            if (is_websocket(nc)) {
                printf("Websocket disconnected");
            }
            break;
        }
    }
}

int main(void) {
    struct mg_mgr mgr;
    struct mg_connection *nc;

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    mg_mgr_init(&mgr, NULL);

    nc = mg_bind(&mgr, s_http_port, ev_handler);
    mg_set_protocol_http_websocket(nc);

    printf("Started on port %s\n", s_http_port);
    while (s_signal_received == 0) {
        mg_mgr_poll(&mgr, 200);
    }
    mg_mgr_free(&mgr);

    return 0;
}