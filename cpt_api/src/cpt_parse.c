//
// Created by jordan on 2022-03-18.
//

#include "cpt_parse.h"

CptResponse * cpt_parse_response(uint8_t * data)
{
    uint8_t data_buf[MD_BUFF_SIZE] = {0};
    CptResponse * res = NULL;
    uint16_t data_size = 0;
    uint8_t code = 0;

    parse(data, PARSE_RES_FMT,
          &code, &data_size, data_buf);

    res = cpt_response_init();
    if ( res )
    {
        res->data = malloc(data_size);
        if ( res->data )
        {
            memmove(res->data, data_buf, data_size);
        }
        res->code = code;
        res->data_size = data_size;
    }

    return res;
}


CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size)
{
    CptRequest * req;
    req = cpt_request_init();
    uint8_t buf[MD_BUFF_SIZE] = {0};
    memset(req, 0, sizeof(struct cpt_request));

    parse( // !
        req_buf, PARSE_REQ_FMT,
        &req->version, &req->command,
        &req->channel_id, &req->msg_len, buf
    );

    req->msg = malloc(req_size);
    if ( req->msg )
    {
        memset(req->msg, 0, req_size);
        memmove(req->msg, buf, req_size);
    }

    return req;
}
