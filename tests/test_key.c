#include "test_key.h"
#include "test_util.h"
#include "ntru.h"
#include "rand.h"
#include "encparams.h"

uint8_t test_key() {
    NtruEncParams params = APR2011_439_FAST;
    NtruEncKeyPair kp;
    ntru_gen_key_pair(&params, &kp, ntru_rand_default);

    /* test public key */
    uint8_t pub_arr[ntru_pub_len(params.N, params.q)];
    ntru_export_pub(&kp.pub, pub_arr);
    NtruEncPubKey pub;
    ntru_import_pub(pub_arr, &pub);
    uint8_t valid = ntru_equals_int(&kp.pub.h, &pub.h);

    /* test private key */
    uint8_t priv_arr[ntru_priv_len(params.df1, params.df2, params.df3)];
    uint8_t priv_len = ntru_export_priv(&kp.priv, priv_arr);
    valid &= priv_len == 105;
    NtruEncPrivKey priv;
    ntru_import_priv(priv_arr, &priv);
    NtruIntPoly t_int1, t_int2;
    ntru_prod_to_int(&priv.t, &t_int1);
    ntru_prod_to_int(&kp.priv.t, &t_int2);
    valid &= ntru_equals_int(&t_int1, &t_int2);

    print_result("test_key", valid);
    return valid;
}
