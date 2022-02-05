#include "argc/types.h"
#include "argc/functions.h"

using namespace argennon;
using namespace ascee;
using namespace argc;

const static long_id balance_chunk = 0;
const static int size_of_balance = 8;

/*
bool create_app_account(long_id address, long_id owner_id) {
    load_chunk_long(address);
    if (!invalid(0,2)) return false;

    resize_chunk(2 + sizeof(pk));????
    store_byte(0 , 8);
    store_pk(2, pk);
    return true;
}
*/
bool create_normal_account(long_id address, publickey_c& pk, signature_c& proof) {
    if (!validate_pk(pk, proof)) return false;
    load_account_chunk(address, 0);
    if (!invalid(0, 2)) return false;
    resize_chunk(2 + sizeof(pk));
    store_int16(0, 8);
    store_pk(0, 2, pk);
    return true;
}

void transfer(long_id from, long_id to, int64 amount, signature_c& sig) {
    //todo: we should check this when we are reading the request and remove this check from here
    if (amount < 0) revert("negative amount");

    int32 sender_balance_offset = -1;

    message_c msg;
    append_str(msg, "{\"to\":");
    append_long_id(msg, to);
    append_str(msg, ",\"amount\":");
    append_int64(msg, amount);
    // "," should not be appended to the end.
    if (!verify_by_acc_once(from, msg, sig, sender_balance_offset)) revert("invalid signature or sender address");

    // loading balance chunk
    load_account_chunk(from, 0);
    // checking that the balance exists
    if (invalid(sender_balance_offset, size_of_balance)) revert("zero balance");
    int64 balance = load_int64(sender_balance_offset);
    if (balance < amount) revert("not enough balance");
    balance -= amount;
    store_int64(sender_balance_offset, balance);
    if (balance == 0) resize_chunk(sender_balance_offset);

    message_c empty;
    int32 recipient_offset = -1;
    if (!verify_by_acc_once(to, empty, sig, recipient_offset)) revert("invalid recipient account");

    // loading nonce chunk of the recipient
    load_account_chunk(to, 0);
    if (invalid(recipient_offset, size_of_balance)) resize_chunk(recipient_offset + size_of_balance);
    add_int64_to(recipient_offset, amount);
}

DEF_ARGC_DISPATCHER {
    int32 position = 0;
    string_view_c method = p_scan_str(request, "", " ", position);
    if (method == "PATCH") {
        long_id account = p_scan_long_id(request, "/balances/", "/", position);
        long_id to = p_scan_long_id(request, "{\"to\":", ",", position);
        int64 amount = p_scan_int64(request, "\"amount\":", ",", position);
        signature_c sig = p_scan_sig(request, R"("sig":")", "\"", position);
        transfer(account, to, amount, sig);
        append_str(response, "success and a good response!");
        return HTTP_OK;
    } else if (method == "PUT") {
        long_id address = p_scan_long_id(request, "/balances/", "/", position);
        publickey_c pk = p_scan_pk(request, R"({"pk":")", "\"", position);
        signature_c proof = p_scan_sig(request, R"("sig":")", "\"", position);
        if (create_normal_account(address, pk, proof)) {
            return HTTP_OK;
        } else {
            return 300;
        }
    }
    return HTTP_OK;
}
