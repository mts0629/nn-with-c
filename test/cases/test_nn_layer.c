/**
 * @file test_nn_layer.c
 * @brief Unit tests of nn_layer.c
 *
 */
#include "nn_layer.h"

#include "unity.h"
#include "test_utils.h"

void setUp(void) {}

void tearDown(void) {}

void test_allocate_and_free(void) {
    NnLayer layer = {
        .params={
            NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=3
        }
    };

    // Test with an identity layer
    TEST_ASSERT_EQUAL_PTR(&layer, nn_layer_alloc_params(&layer));
    TEST_ASSERT_NOT_NULL(layer.x);
    TEST_ASSERT_NOT_NULL(layer.y);
    TEST_ASSERT_NULL(layer.w);
    TEST_ASSERT_NULL(layer.b);
    TEST_ASSERT_NOT_NULL(layer.dx);
    TEST_ASSERT_NULL(layer.dw);
    TEST_ASSERT_NULL(layer.db);
    TEST_ASSERT_EQUAL_PTR(identity_forward, layer.forward);
    TEST_ASSERT_EQUAL_PTR(identity_backward, layer.backward);

    nn_layer_free_params(&layer);
    TEST_ASSERT_NULL(layer.x);
    TEST_ASSERT_NULL(layer.y);
    TEST_ASSERT_NULL(layer.w);
    TEST_ASSERT_NULL(layer.b);
    TEST_ASSERT_NULL(layer.dx);
    TEST_ASSERT_NULL(layer.dw);
    TEST_ASSERT_NULL(layer.db);
    TEST_ASSERT_NULL(layer.forward);
    TEST_ASSERT_NULL(layer.backward);
}

void test_allocation_fail_if_layer_is_NULL(void) {
    TEST_ASSERT_NULL(nn_layer_alloc_params(NULL));
}

void test_allocation_fail_if_parameters_contains_0(void) {
    NnLayer layer[3] = {
        { .params={ NN_LAYER_TYPE_IDENTITY, .batch_size=0, .in=2, .out=3} },
        { .params={ NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=0, .out=3} },
        { .params={ NN_LAYER_TYPE_IDENTITY, .batch_size=1, .in=2, .out=0} }
    };

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_NULL(nn_layer_alloc_params(&layer[i]));
        TEST_ASSERT_NULL(layer[i].x);
        TEST_ASSERT_NULL(layer[i].y);
        TEST_ASSERT_NULL(layer[i].w);
        TEST_ASSERT_NULL(layer[i].b);
        TEST_ASSERT_NULL(layer[i].dx);
        TEST_ASSERT_NULL(layer[i].dw);
        TEST_ASSERT_NULL(layer[i].db);
        TEST_ASSERT_NULL(layer[i].forward);
        TEST_ASSERT_NULL(layer[i].backward);
    }
}

void test_free_to_NULL(void) {
    nn_layer_free_params(NULL);
}

void test_connect(void) {
    NnLayer layer = {
        .params={ .batch_size=8, .in=2, .out=10 }
    };

    NnLayer next_layer = {
        .params={ .out=3 }
    };

    nn_layer_connect(&layer, &next_layer);

    TEST_ASSERT_EQUAL_INT(8, next_layer.params.batch_size);
    TEST_ASSERT_EQUAL_INT(10, next_layer.params.in);
}

float *dummy_forward(NnLayer *layer, const float *x) {
    for (int i = 0 ; i < 3; i++) {
        layer->y[i] = x[i] * 2;
    }
    return layer->y;
}

void test_forward(void) {
    NnLayer layer = {
        .y = FLOAT_ZEROS(3),
        .forward = dummy_forward
    };

    float answer[] = { -2, 0, 2 };

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        answer, nn_layer_forward(&layer, FLOAT_ARRAY(-1, 0, 1)), 3
    );
}

void test_forward_fail_if_layer_is_NULL(void) {
    TEST_ASSERT_NULL(nn_layer_forward(NULL, FLOAT_ZEROS(1)));
}

void test_forward_fail_if_x_is_NULL(void) {
    NnLayer layer = {
        .y = FLOAT_ZEROS(3),
        .forward = dummy_forward
    };

    TEST_ASSERT_NULL(nn_layer_forward(&layer, NULL));
}

/*
void test_backward(void) {
    NnLayer layer = {
        .batch_size = 1,
        .in = 2,
        .out = 3,
        .x = FLOAT_ZEROS(2),
        .y = FLOAT_ZEROS(3),
        .z = FLOAT_ZEROS(3),
        .w = FLOAT_ZEROS(3 * 2),
        .b = FLOAT_ZEROS(3),
        .dx = FLOAT_ZEROS(2),
        .dz = FLOAT_ZEROS(3),
        .dw = FLOAT_ZEROS(3 * 2),
        .db = FLOAT_ZEROS(3),
        .forward = forward,
        .backward = backward
    };

    COPY_ARRAY(
        layer.w,
        FLOAT_ARRAY(
            0, 1, 2,
            3, 4, 5
        )
    );

    COPY_ARRAY(
        layer.b,
        FLOAT_ARRAY(1, 1, 1)
    );

    nn_layer_forward(&layer, FLOAT_ARRAY(1, 1));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(0.003130589f, 0.01056396f),
        nn_layer_backward(
            &layer,
            FLOAT_ARRAY(-0.01798624f, 0.99752736f, 0.99966466f)
        ),
        2
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(
            -0.0003176862f, 0.002460367f, 0.0003351109f,
            -0.0003176862f, 0.002460367f, 0.0003351109f
        ),
        layer.dw,
        (3 * 2)
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(-0.0003176862f, 0.002460367f, 0.0003351109f),
        layer.db,
        3
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(-0.0003176862f, 0.002460367f, 0.0003351109f),
        layer.dz,
        3
    );
}

void test_backward_batch(void) {
    NnLayer layer = {
        .batch_size = 2,
        .in = 2,
        .out = 3,
        .x = FLOAT_ZEROS(2 * 2),
        .y = FLOAT_ZEROS(2 * 3),
        .z = FLOAT_ZEROS(2 * 3),
        .w = FLOAT_ZEROS(3 * 2),
        .b = FLOAT_ZEROS(3),
        .dx = FLOAT_ZEROS(2 * 2),
        .dz = FLOAT_ZEROS(2 * 3),
        .dw = FLOAT_ZEROS(3 * 2),
        .db = FLOAT_ZEROS(3),
        .forward = forward,
        .backward = backward
    };

    COPY_ARRAY(
        layer.w,
        FLOAT_ARRAY(
            0, 1, 2,
            3, 4, 5
        )
    );

    COPY_ARRAY(
        layer.b,
        FLOAT_ARRAY(1, 1, 1)
    );

    nn_layer_forward(
        &layer,
        FLOAT_ARRAY(
            1, 1,
            0, -1
        )
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(
            0.003130589f, 0.01056396f,
            -0.04239874f, -0.1330014f
        ),
        nn_layer_backward(
            &layer,
            // dy = y - [1, 0, 0]
            //          [0, 1, 0]
            FLOAT_ARRAY(
                -0.01798624f, 0.99752736f, 0.99966466f,
                 0.119203f, -0.952574f, 0.0179862f
            )
        ),
        (2 * 2)
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(
            -0.0003176862f, 0.002460367f, 0.0003351109f,
            -0.01283324f, 0.04549448f, 0.00001742589f
        ),
        layer.dw,
        (2 * 3)
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(0.01219786f, -0.04057374f, 0.0006527959f),
        layer.db,
        3
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(
            -0.0003176862f, 0.002460367f, 0.0003351109f,
            0.01251555f, -0.04303412f, 0.000317685f
        ),
        layer.dz,
        (2 * 3)
    );
}

void test_backward_fail_if_layer_is_NULL(void) {
    TEST_ASSERT_NULL(nn_layer_backward(NULL, FLOAT_ZEROS(1)));
}

void test_backward_fail_if_dy_is_NULL(void) {
    NnLayer layer = {
        .batch_size = 1,
        .in = 2,
        .out = 3,
        .x = FLOAT_ZEROS(2),
        .y = FLOAT_ZEROS(3),
        .z = FLOAT_ZEROS(3),
        .w = FLOAT_ZEROS(3 * 2),
        .b = FLOAT_ZEROS(3),
        .dx = FLOAT_ZEROS(2),
        .dz = FLOAT_ZEROS(3),
        .dw = FLOAT_ZEROS(3 * 2),
        .db = FLOAT_ZEROS(3),
        .forward = forward,
        .backward = backward
    };

    TEST_ASSERT_NULL(nn_layer_backward(&layer, NULL));
}

void test_update(void) {
    NnLayer layer = {
        .in = 2,
        .out = 3,
        .x = FLOAT_ZEROS(2),
        .y = FLOAT_ZEROS(3),
        .z = FLOAT_ZEROS(3),
        .w = FLOAT_ZEROS(3 * 2),
        .b = FLOAT_ZEROS(3),
        .dx = FLOAT_ZEROS(2),
        .dz = FLOAT_ZEROS(3),
        .dw = FLOAT_ZEROS(3 * 2),
        .db = FLOAT_ZEROS(3),
        .forward = forward,
        .backward = backward
    };

    COPY_ARRAY(
        layer.w,
        FLOAT_ARRAY(
            1, 1, 1,
            1, 1, 1
        )
    );

    COPY_ARRAY(
        layer.dw,
        FLOAT_ARRAY(
            1, 2, 3,
            4, 5, 6
        )
    );

    COPY_ARRAY(
        layer.b,
        FLOAT_ARRAY(1, 1, 1)
    );

    COPY_ARRAY(
        layer.db,
        FLOAT_ARRAY(1, 2, 3)
    );

    nn_layer_update(&layer, 0.01);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(
            0.99, 0.98, 0.97,
            0.96, 0.95, 0.94
        ),
        layer.w,
        (3 * 2)
    );

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(
        FLOAT_ARRAY(0.99, 0.98, 0.97),
        layer.b,
        3
    );
}
*/
