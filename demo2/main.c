#include "skf.h"
#include <string.h>

#ifndef _WIN32
#include <dlfcn.h>
#include <unistd.h>
#if !defined(PATH_MAX)
#include <limits.h>
#  if !defined(_POSIX_C_SOURCE)
#    warning "_POSIX_C_SOURCE is disabled! But we need it to get the value of PATH_MAX"
#    define PATH_MAX 260
#  endif
#endif
#define MAX_PATH PATH_MAX
#else
#include <windows.h>
#include <tchar.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


PSKF_FUNCLIST FunctionList;

int load_library()
{
    int ret = 0;
    void * lib_handle = NULL;
    char  path[MAX_PATH] = {0};

#ifdef _WIN32
    P_SKF_GetFuncList get_func_list = NULL;
    TCHAR szUnicodePath[MAX_PATH+16];
    const TCHAR *szDllFileName = _T("SKF_usb_ms_x86_64_19.1128_with_log.dll");

#if defined(UNICODE)
    _tprintf(_T("(Note: Unicode mode for Visual Studio C/C++ compiler is enabled)\n"));
#endif
    (void)path;
    GetCurrentDirectory(MAX_PATH, szUnicodePath);
    _tcscat_s(szUnicodePath, MAX_PATH, _T("\\..\\demo2\\"));
#if !defined(_M_AMD64)
    {
        szDllFileName = _T("SKF_usb_ms_i686_19.1128_with_log.dll");
    }
#endif
    _tcscat_s(szUnicodePath, MAX_PATH, szDllFileName);
    _tprintf(_T("Load Dll %s\n"), szUnicodePath);
    lib_handle = LoadLibrary((void*)szUnicodePath);
    if (lib_handle==NULL)
    {
        char szTmp[512];
        ret = GetLastError();
        printf("Load Dll Fail:%d\n", ret);
        strerror_s(szTmp, 256, ret);
        printf("%s\n", szTmp);
        return ret;
    }
    else
    {
        get_func_list = (P_SKF_GetFuncList)GetProcAddress(lib_handle, "SKF_GetFuncList");
        if (get_func_list == NULL)
        {
            ret = GetLastError();
            return ret;
        }
        printf("Load Dll OK\n");
        ret = get_func_list(&FunctionList);
        if (ret) {
            fprintf(stderr, "SKF_GetFuncList() failed: ERROR=%#x\n", ret);
            return ret;
        }
    }
#else
    P_SKF_GetFuncList get_func_list;

    getcwd(path, sizeof(path));
    strcat(path, "/libskf.so");
    lib_handle = dlopen(path, RTLD_LAZY );
    if (!lib_handle)
    {
        printf("Open Error:%s.\n", dlerror());
        return 0;
    }

    get_func_list = dlsym(lib_handle, "SKF_GetFuncList");
    if (get_func_list == NULL)
    {
        printf("Dlsym Error:%s.\n", dlerror());
        return 0;
    }

    ret = get_func_list(&FunctionList);
    if (ret) {
        fprintf(stderr, "SKF_GetFuncList() failed: ERROR=%#x\n", ret);
        return ret;
    }
#endif

    return ret;
}

void print_data(char *info, unsigned char *data, unsigned int len)
{
    unsigned int i = 0;
    if (info) printf("%s\n", info);

    for (i = 0; i < len; i++) {
        if (i && i % 16 == 0) printf("\n");
        printf("0x%02x ", data[i]);
    }
    printf("\n");
}

int sym_gdb_test(HANDLE hdev, unsigned int algo, unsigned char *key, unsigned char *iv, unsigned int iv_len, unsigned char *data, unsigned int data_len, unsigned char *gdb_data, unsigned int gdb_data_len)
{
    int ret = 0;
    HANDLE hkey;
    BLOCKCIPHERPARAM bp;
    u8 enc_data[1024] = {0};
    u32 enc_len = 0, enc_final_len = 0;
    u8 dec_data[1024] = {0};
    u32 dec_len = 0, dec_final_len = 0;

    ret = FunctionList->SKF_SetSymmKey(hdev, key, algo, &hkey);
    if (ret) {
        fprintf(stderr, "SKF_SetSymmKey() failed: %#x\n", ret);
        return ret;
    }

    memset(&bp, 0, sizeof(bp));
    bp.IVLen = iv_len;
    if (bp.IVLen > 0) {
        if (!iv) {
            printf("sym cbc mode, but iv is null\n");
            ret = -1;
            goto end;
        }

        memcpy(bp.IV, iv, bp.IVLen);
    }

    ret = FunctionList->SKF_EncryptInit(hkey, bp);
    if (ret) {
        fprintf(stderr, "SKF_EncryptInit() failed: %#x\n", ret);
        goto end;
    }

    ret = FunctionList->SKF_EncryptUpdate(hkey, data, data_len, enc_data, &enc_len);
    if (ret) {
        fprintf(stderr, "SKF_EncryptInit() failed: %#x\n", ret);
        goto end;
    }

    ret = FunctionList->SKF_EncryptFinal(hkey, enc_data + enc_len, &enc_final_len);
    if (ret) {
        fprintf(stderr, "SKF_EncryptInit() failed: %#x\n", ret);
        goto end;
    }
    enc_len += enc_final_len;

    /**
     * show original data and encrypted data
     */
    print_data("original data: ", data, data_len);
    print_data("encrypted data: ", enc_data, enc_len);

    if (gdb_data && gdb_data_len > 0 && (enc_len != gdb_data_len || memcmp(gdb_data, enc_data, enc_len))) {
        printf("enc and gdb data is not same!\n");
        ret = -1;
        goto end;
    }

    ret = FunctionList->SKF_DecryptInit(hkey, bp);
    if (ret) {
        fprintf(stderr, "SKF_DecryptInit() failed: %#x\n", ret);
        goto end;
    }

    ret = FunctionList->SKF_DecryptUpdate(hkey, enc_data, enc_len, dec_data, &dec_len);
    if (ret) {
        fprintf(stderr, "SKF_EncryptInit() failed: %#x\n", ret);
        goto end;
    }

    ret = FunctionList->SKF_DecryptFinal(hkey, dec_data + dec_len, &dec_final_len);
    if (ret) {
        fprintf(stderr, "SKF_EncryptInit() failed: %#x\n", ret);
        goto end;
    }
    dec_len += dec_final_len;

    /**
     * show encrypted data and decrypted data
     */
    print_data("decrypted data: ", dec_data, dec_len);

    if (enc_len != dec_len || memcmp(data, dec_data, enc_len)) {
        printf("enc and dec data is not same!\n");
        ret = -1;
    } else {
        printf("sym enc and dec succ!\n");
    }

end:
    FunctionList->SKF_CloseHandle(hkey);
    return ret;
}

int sm4_ecb_gdb_data_test(HANDLE hdev)
{
    int ret = 0;
    unsigned char key[] = {0x77,0x7f,0x23,0xc6,0xfe,0x7b,0x48,0x73,0xdd,0x59,0x5c,0xff,0xf6,0x5f,0x58,0xec};
    unsigned char data[] = {0x5f,0xe9,0x7c,0xcd,0x58,0xfe,0xd7,0xab,0x41,0xf7,0x1e,0xfb,0xfd,0xe7,0xe1,0x46};
    unsigned char gdb_data[] = {0x56,0xda,0x23,0xe2,0x5f,0xa7,0xcd,0x82,0x5d,0x51,0xc2,0x20,0xf5,0x98,0x09,0x0b};

    ret = sym_gdb_test(hdev, SGD_SMS4_ECB, key, NULL, 0, data, sizeof(data), gdb_data, sizeof(gdb_data));
    return ret;
}

int get_device_info(HANDLE hdev)
{
    int ret = 0;
    DEVINFO info;

    memset(&info, 0, sizeof(info));
    ret = FunctionList->SKF_GetDevInfo(hdev, &info);
    if (ret) {
        fprintf(stderr, "SKF_GetDevInfo() failed: %#x\n", ret);
        return ret;
    }

    printf("Manufacturer: %s\n", info.Manufacturer);
    printf("Issuer: %s\n", info.Issuer);
    printf("Label: %s\n", info.Label);

    char buf[32+1];
    buf[32] = '\0';
    memcpy(buf, &(info.SerialNumber[0]), 32);
    printf("SerialNumber: %s\n", buf);

    return ret;
}

int main(int agrc, char *agrv[])
{
    int ret = 0;
    char devices[128] = {0};
    u32 devices_size = sizeof(devices);
    DEVHANDLE hdev;

    ret = load_library();
    if (ret) {
        fprintf(stderr, "load_library() failed: %#x\n", ret);
        return ret;
    }

    hdev = 0;//hdev=UNDEFINED_HANDLE;
    ret = FunctionList->SKF_EnumDev(1, devices, &devices_size);
    if (ret) {
        fprintf(stderr, "SKF_EnumDev() failed: %#x\n", ret);
        goto end;
    }

    ret = FunctionList->SKF_ConnectDev(devices, &hdev);
    if (ret) {
        fprintf(stderr, "SKF_ConnectDev() failed: %#x\n", ret);
        goto end;
    }

    ret = get_device_info(hdev);
    if (ret) {
        fprintf(stderr, "get device info failed: %#x\n", ret);
        goto end;
    }

    ret = sm4_ecb_gdb_data_test(hdev);
    if (ret) {
        fprintf(stderr, "sym_test() failed: %#x\n", ret);
    }

end:
    if (hdev) {
        ret = FunctionList->SKF_DisConnectDev(hdev);
    }

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
