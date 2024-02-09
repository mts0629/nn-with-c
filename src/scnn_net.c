/**
 * @file scnn_net.c
 * @brief Network structure
 * 
 */
#include "scnn_net.h"

#include <stdlib.h>
#include <stdbool.h>

int scnn_net_size(const scnn_net *net)
{
    return net->size;
}

int scnn_net_batch_size(const scnn_net *net)
{
    return net->batch_size;
}

scnn_layer *scnn_net_layers(scnn_net *net)
{
    return net->layers;
}

scnn_layer* scnn_net_input(const scnn_net *net)
{
    return net->input;
}

scnn_layer* scnn_net_output(const scnn_net *net)
{
    return net->output;
}

scnn_net *scnn_net_alloc(void)
{
    scnn_net *net = malloc(sizeof(scnn_net));
    if (net == NULL) {
        return NULL;
    }

    net->size = 0;
    net->batch_size = 1;
    net->layers = NULL;
    net->input = NULL;
    net->output = NULL;

    return net;
}

scnn_net *scnn_net_append(scnn_net *net, scnn_layer_params params)
{
    if (net == NULL) {
        return NULL;
    }

    // Reallocate and extend layers in the network
    scnn_layer *realloc_layers = realloc(net->layers, sizeof(scnn_layer) * (net->size + 1));
    if (realloc_layers == NULL) {
        return NULL;
    }

    net->layers = realloc_layers;
    realloc_layers = NULL;

    // Initialize new layer
    scnn_layer *layer = &net->layers[net->size];
    layer->params = params;
    layer->x = NULL;
    layer->y = NULL;
    layer->w = NULL;
    layer->b = NULL;
    layer->dx = NULL;
    layer->dw = NULL;
    layer->db = NULL;

    // Connect the layer
    scnn_layer_connect(net->output, &net->layers[net->size]);

    // Set the first layer as a network input
    net->input = &net->layers[0];
    // And the last layer as a network output
    net->output = layer;

    net->size++;

    return net;
}

scnn_net *scnn_net_init(scnn_net *net)
{
    if ((net == NULL) || (net->size == 0)) {
        return NULL;
    }

    for (int i = 0; i < net->size; i++) {
        if (scnn_layer_init(&net->layers[i]) == NULL) {
            return NULL;
        }
    }

    return net;
}

float *scnn_net_forward(scnn_net *net, const float *x)
{
    if ((net == NULL) || (x == NULL)) {
        return NULL;
    }

    float *in = (float*)x;
    float *out;
    for (int i = 0; i < net->size; i++) {
        out = scnn_layer_forward(&net->layers[i], in);
        in = out;
    }

    return out;
}

float *scnn_net_backward(scnn_net *net, const float *dy)
{
    if ((net == NULL) || (dy == NULL)) {
        return NULL;
    }

    float *din = (float*)dy;
    float *dout;
    for (int i = (net->size - 1); i >= 0; i--) {
        dout = scnn_layer_backward(&net->layers[i], din);
        din = dout;
    }

    return dout;
}

void net_update(scnn_net *net, const float learning_rate)
{
    for (int i = 0; i < net->size; i++) {
        layer_update(&net->layers[i], learning_rate);
    }
}

void scnn_net_free(scnn_net **net)
{
    if ((net == NULL) || (*net == NULL)) {
        return;
    }

    scnn_net *instance = *net;

    for (int i = 0; i < instance->size; i++) {
        free(instance->layers[i].x);
        instance->layers[i].x = NULL;
        free(instance->layers[i].y);
        instance->layers[i].y = NULL;
        free(instance->layers[i].w);
        instance->layers[i].w = NULL;
        free(instance->layers[i].b);
        instance->layers[i].b = NULL;
        free(instance->layers[i].dx);
        instance->layers[i].dx = NULL;
        free(instance->layers[i].dw);
        instance->layers[i].dw = NULL;
        free(instance->layers[i].db);
        instance->layers[i].db = NULL;
    }

    free(instance->layers);
    instance->layers = NULL;

    free(*net);
    *net = NULL;
}
