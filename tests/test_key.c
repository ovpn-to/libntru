#include "test_key.h"
#include "test_util.h"
#include "ntru.h"
#include "rand.h"
#include "encparams.h"

uint8_t test_key() {
#ifndef NTRU_AVOID_HAMMING_WT_PATENT
    NtruEncParams param_arr[] = {EES439EP1, EES1087EP2};
#else
    NtruEncParams param_arr[] = {EES1087EP2};
#endif   /* NTRU_AVOID_HAMMING_WT_PATENT */
    uint8_t valid = 1;

    uint8_t i;
    for (i=0; i<sizeof(param_arr)/sizeof(param_arr[0]); i++) {
        NtruEncParams params = param_arr[i];
        NtruEncKeyPair kp;
        ntru_gen_key_pair(&params, &kp, ntru_rand_default);

        /* test public key */
        uint8_t pub_arr[ntru_pub_len(&params)];
        ntru_export_pub(&kp.pub, pub_arr);
        NtruEncPubKey pub;
        ntru_import_pub(pub_arr, &pub);
        valid &= ntru_equals_int(&kp.pub.h, &pub.h);

        /* test private key */
        uint8_t priv_arr[ntru_priv_len(&params)];
        uint16_t priv_len = ntru_export_priv(&kp.priv, priv_arr);
        valid &= priv_len == ntru_priv_len(&params);
        NtruEncPrivKey priv;
        ntru_import_priv(priv_arr, &priv);
        NtruIntPoly t_int1, t_int2;
#ifndef NTRU_AVOID_HAMMING_WT_PATENT
        if (params.prod_flag) {
            ntru_prod_to_int(&priv.t.prod, &t_int1);
            ntru_prod_to_int(&kp.priv.t.prod, &t_int2);
        }
        else
#endif   /* NTRU_AVOID_HAMMING_WT_PATENT */
        {
            ntru_tern_to_int(&priv.t.tern, &t_int1);
            ntru_tern_to_int(&kp.priv.t.tern, &t_int2);
        }
        valid &= ntru_equals_int(&t_int1, &t_int2);
    }

    print_result("test_key", valid);
    return valid;
}
