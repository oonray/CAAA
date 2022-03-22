#include "ssl.h"

AsocSSLConfig *AsocSSLConfig_New(bstring folder){
    ioStream *stream = NULL;

    AsocSSLConfig *conf = calloc(1,sizeof(AsocSSLConfig));
    check(conf!=NULL,"Cold not allocate conf");

    conf->init = -1;

    if(IoFileStream_DirectoryExists(folder)<0){
        check(IoFileStream_DirectoryCreate(folder,RIGHTS)>=0,"Config dir could not be created");
    }


    bstring path = bformat("%s%s",bdata(folder),CONFIG_FILE);

    int rc = IoFileStream_FileExists(path);
    if(rc<0){
        int fd = IoFileStream_FileCreate(path,RIGHTS);
        check(fd>=0,"Config file could not be created");
        close(fd);
        log_info("File %s Created",bdata(path));
    }

    stream = NewIoStreamFile(path,O_RDWR,RIGHTS,10*1024);
    check(IoStreamBuffWrite(stream,bfromcstr(EXAMPLE_CONTENT))>=0,"Could not write example text to buffer");
    check(IoStreamIoWrite(stream)>=0,"Could not write to file fd");
    DestroyIoStream(stream);

    char errbuff[200];
    FILE *f = fopen(bdata(path),"r");
    check(f,"Could not open file %s",bdata(path));

    conf->conf_data = toml_parse_file(f,errbuff,sizeof(errbuff));
    toml_table_t *ssl = toml_table_in(conf->conf_data,"ssl");
    check(ssl!=NULL,"Could not fetch ssl, check config");
    fclose(f);

    toml_datum_t pki = toml_string_in(ssl,"pki");
    toml_datum_t cert = toml_string_in(ssl,"cert");

    conf->pki = bfromcstr(pki.u.s);
    conf->cert = bfromcstr(cert.u.s);

    return conf;
error:
    if(stream!=NULL){
        DestroyIoStream(stream);
    }
    if(conf!=NULL)
        free(conf);
    return NULL;
}

void AsocSSL_Destroy(AsocSSL *conf){
    free(conf);
}
