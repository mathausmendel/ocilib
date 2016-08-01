#include "ocilib.h"

/* requires script demo/ref.sql */

#define SIZE_STR 100

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void dump_ref(OCI_Ref *ref)
{
    OCI_Object *vendor;
    char data[SIZE_STR + 1];

    /* print ref hexadecimal value */

    OCI_RefToText(ref, SIZE_STR, data);
    printf("...Ref Hex value : %s\n", data);

    /* get object from ref */

    vendor = OCI_RefGetObject(ref);

    /* print object values */

    printf("...%i - %s\n", OCI_ObjectGetInt(vendor, "code"), OCI_ObjectGetString(vendor, "name"));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;
    OCI_Ref       *ref;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select ref(v) from vendors v");
    rs = OCI_GetResultset(st);

    printf("\n\n=> fetch refs from object table\n\n");

    while (OCI_FetchNext(rs))
    {
        dump_ref(OCI_GetRef(rs, 1));
    }

    printf("\n\n=> bind a local ref object to a PL/SQL statement\n\n");

    ref = OCI_RefCreate(cn, OCI_TypeInfoGet(cn, "vendor_t", OCI_TIF_TYPE));

    OCI_Prepare(st, "begin select ref(v) into :vendor from vendors v where v.code = 1; end; ");

    OCI_BindRef(st, ":vendor", ref);
    OCI_Execute(st);

    dump_ref(ref);

    OCI_RefFree(ref);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
