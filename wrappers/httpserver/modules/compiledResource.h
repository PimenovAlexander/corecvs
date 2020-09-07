//
// Created by dio on 9/7/20.
//

#ifndef CORECVS_COMPILEDRESOURCE_H
#define CORECVS_COMPILEDRESOURCE_H

struct CompiledResource {
    CompiledResource(const unsigned char *data = nullptr, int length = 0) : data(data), length(length) {}

    const unsigned char *data;
    int length;
};

#endif //CORECVS_COMPILEDRESOURCE_H
