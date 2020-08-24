#ifndef C_STYLE_EXAMPLE_H
#define C_STYLE_EXAMPLE_H


void on_get_index(struct evhttp_request *req, void *arg);

void on_get_test_data(struct evhttp_request *req, void *arg);
void on_get_test_image(struct evhttp_request *req, void *arg);


void on_image_request(struct evhttp_request *req, void *arg);
void on_get_stats_request(struct evhttp_request *req, void *arg);
void on_change_stats_request(struct evhttp_request *req, void *arg);
void on_long_poll(struct evhttp_request *req, void *arg);
void on_other_requests(struct evhttp_request * req, void *arg);


void on_long_pull_request_subscribe(struct evhttp_request *req, void *arg);
void on_long_poll_request_announce(struct evhttp_request *req, void *arg);



#endif // C_STYLE_EXAMPLE_H
