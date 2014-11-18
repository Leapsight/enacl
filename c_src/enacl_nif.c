#include "erl_nif.h"

#include <sodium.h>

static
ERL_NIF_TERM nacl_error_tuple(ErlNifEnv *env, char *error_atom) {
	return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, error_atom));
}

static
ERL_NIF_TERM enif_crypto_hash(ErlNifEnv *env, int argc, ERL_NIF_TERM const argv[]) {
	ErlNifBinary input;
	ErlNifBinary result;
	
	if ((argc != 1) || (!enif_inspect_iolist_as_binary(env, argv[0], &input))) {
		return enif_make_badarg(env);
	}
	
	if (!enif_alloc_binary(crypto_hash_BYTES, &result)) {
		return nacl_error_tuple(env, "alloc_failed");
	}
	
	crypto_hash(result.data, input.data, input.size);
	
	return enif_make_binary(env, &result);
}

static
ERL_NIF_TERM enif_crypto_box_keypair(ErlNifEnv *env, int argc, ERL_NIF_TERM const argv[]) {
	ErlNifBinary pk, sk;
	
	if (argc != 0) {
		return enif_make_badarg(env);
	}
	
	if (!enif_alloc_binary(crypto_box_PUBLICKEYBYTES, &pk)) {
		return nacl_error_tuple(env, "alloc_failed");
	}
	
	if (!enif_alloc_binary(crypto_box_SECRETKEYBYTES, &sk)) {
		return nacl_error_tuple(env, "alloc_failed");
	}
	
	crypto_box_keypair(pk.data, sk.data);
	
	return enif_make_tuple3(env, enif_make_atom(env, "ok"), enif_make_binary(env, &pk), enif_make_binary(env, &sk));
}

static ErlNifFunc nif_funcs[] = {
	{"crypto_box_keypair", 0, enif_crypto_box_keypair},
	{"crypto_hash", 1, enif_crypto_hash, ERL_NIF_DIRTY_JOB_CPU_BOUND}
};

ERL_NIF_INIT(enacl_nif, nif_funcs, NULL, NULL, NULL, NULL);
