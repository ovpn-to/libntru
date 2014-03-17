#include <string.h>
#include <stdint.h>
#ifdef WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "poly.h"
#include "encparams.h"
#include "types.h"

void ntru_export_pub(NtruEncPubKey *key, uint8_t *arr) {
    /* write N */
    uint16_t N_endian = htons(key->h.N);
    memcpy(arr, &N_endian, sizeof N_endian);
    arr += sizeof N_endian;

    /* write q */
    uint16_t q_endian = htons(key->q);
    memcpy(arr, &q_endian, sizeof q_endian);
    arr += sizeof q_endian;

    /* write h */
    ntru_to_arr(&key->h, key->q, arr);
}

uint16_t ntru_import_pub(uint8_t *arr, NtruEncPubKey *key) {
    uint8_t *arr_head = arr;

    /* read N */
    uint16_t N_endian;
    memcpy(&N_endian, arr_head, sizeof N_endian);
    uint16_t N = ntohs(N_endian);
    key->h.N = N;

    /* read q */
    arr_head += sizeof N_endian;
    uint16_t q_endian;
    memcpy(&q_endian, arr_head, sizeof q_endian);
    uint16_t q = ntohs(q_endian);
    key->q = q;
    arr_head += sizeof q_endian;

    /* read h */
    ntru_from_arr(arr_head, N, q, &key->h);
    arr_head += ntru_enc_len(N, q);

    return arr_head - arr;
}

uint16_t ntru_pub_len(uint16_t N, uint16_t q) {
    return 4 + ntru_enc_len(N, q);
}

uint16_t ntru_tern_to_arr(NtruTernPoly *poly, uint8_t *arr) {
    uint8_t *arr_head = arr;

    /* write #ones and #neg_ones */
    uint16_t num_ones = htons(poly->num_ones);
    memcpy(arr_head, &num_ones, sizeof num_ones);
    arr_head += sizeof num_ones;
    uint16_t num_neg_ones = htons(poly->num_neg_ones);
    memcpy(arr_head, &num_neg_ones, sizeof num_neg_ones);
    arr_head += sizeof num_neg_ones;

    /* write indices of ones and negative ones */
    uint16_t i;
    for (i=0; i<poly->num_ones; i++) {
        uint16_t idx = htons(poly->ones[i]);
        memcpy(arr_head, &idx, sizeof idx);
        arr_head += sizeof idx;
    }
    for (i=0; i<poly->num_neg_ones; i++) {
        uint16_t idx = htons(poly->neg_ones[i]);
        memcpy(arr_head, &idx, sizeof idx);
        arr_head += sizeof idx;
    }

    return arr_head - arr;
}

uint16_t ntru_export_priv(NtruEncPrivKey *key, uint8_t *arr) {
    uint8_t *arr_head = arr;

    /* write N */
    uint16_t N_endian = htons(key->t.N);
    memcpy(arr_head, &N_endian, sizeof N_endian);
    arr_head += sizeof N_endian;

    /* write q */
    uint16_t q_endian = htons(key->q);
    memcpy(arr_head, &q_endian, sizeof q_endian);
    arr_head += sizeof q_endian;

    /* write flags */
    *arr_head = 3;
    arr_head++;

    /* write f1, f2, f3 */
    arr_head += ntru_tern_to_arr(&key->t.f1, arr_head);
    arr_head += ntru_tern_to_arr(&key->t.f2, arr_head);
    arr_head += ntru_tern_to_arr(&key->t.f3, arr_head);

    return arr_head - arr;
}

uint16_t ntru_tern_from_arr(uint8_t *arr, uint16_t N, NtruTernPoly *poly) {
    poly->N = N;
    uint8_t *arr_head = arr;

    /* read #ones and #ones */
    uint16_t num_ones;
    memcpy(&num_ones, arr_head, sizeof num_ones);
    poly->num_ones = ntohs(num_ones);
    arr_head += sizeof num_ones;
    uint16_t num_neg_ones;
    memcpy(&num_neg_ones, arr_head, sizeof num_neg_ones);
    poly->num_neg_ones = ntohs(num_neg_ones);
    arr_head += sizeof num_neg_ones;

    /* read indices of ones and negative ones */
    uint16_t i;
    for (i=0; i<poly->num_ones; i++) {
        uint16_t idx;
        memcpy(&idx, arr_head, sizeof idx);
        poly->ones[i] = ntohs(idx);
        arr_head += sizeof idx;
    }
    for (i=0; i<poly->num_neg_ones; i++) {
        uint16_t idx;
        memcpy(&idx, arr_head, sizeof idx);
        poly->neg_ones[i] = ntohs(idx);
        arr_head += sizeof idx;
    }

    return arr_head - arr;
}

void ntru_import_priv(uint8_t *arr, NtruEncPrivKey *key) {
    /* read N */
    uint16_t N;
    memcpy(&N, arr, sizeof N);
    key->t.N = ntohs(N);
    arr += sizeof N;

    /* read q */
    uint16_t q;
    memcpy(&q, arr, sizeof q);
    key->q = ntohs(q);
    arr += sizeof q;

    /* skip flags */
    arr++;

    arr += ntru_tern_from_arr(arr, key->t.N, &key->t.f1);
    arr += ntru_tern_from_arr(arr, key->t.N, &key->t.f2);
    arr += ntru_tern_from_arr(arr, key->t.N, &key->t.f3);
}

uint16_t ntru_priv_len(uint16_t df1, uint16_t df2, uint16_t df3) {
    return 5 + 4 + 4*df1 + 4 + 4*df2 + 4 + 4*df3;
}
