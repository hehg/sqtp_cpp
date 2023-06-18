#include "os/sq_hardware.h"
#include "sq_define.h"

#ifdef WINDOWS
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/ioctl.h>
#include <netdb.h>
#include <net/if.h>
#endif
namespace sq
{
#ifdef WINDOWS
	 void get_mac_address(const char* ip,std::string&address)
	{
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(AdapterInfo);
		char mac_addr[512] = { 0 };

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");

		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

			free(AdapterInfo);
			AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
			if (AdapterInfo == NULL) {
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
			do {
				sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
				if (strcmp(ip, pAdapterInfo->IpAddressList.IpAddress.String) == 0)
				{
					printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
					break;
				}
				


				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);
		}
		free(AdapterInfo);
		address = mac_addr;
	}
#elif LINUX
	 void get_mac_address(const char*ip, std::string&address)
	 {
		 int i, sockfd;
		 struct ifreq ifr;
		 char mac[30];

		 sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd == -1) {
			 printf("scokfd create");
			 return;
		 }

		 memset(&ifr,0, sizeof(ifr));
		 strncpy(ifr.ifr_name, ip, sizeof(ifr.ifr_name));

		 if ((ioctl(sockfd, SIOCGIFHWADDR, &ifr))<0) {
			 printf("ioctl error\n");
			 return;
		 }

		 for (i = 0; i<6; i++)
			 sprintf(mac + 3 * i, "%02x:", (unsigned char)ifr.ifr_hwaddr.sa_data[i]);
		 printf("MAC: %s \n", mac);
		 address = mac;
	 }
#endif

void  get_disk_serial_num(string&serial_id)
	{
#if 0
#if defined(WINDOWS)
		DWORD Serial;
		DWORD Length;
		GetVolumeInformation("C:\\", NULL, MAX_PATH, &Serial, &Length, NULL, NULL, MAX_PATH);

		stringstream strstream;
		strstream << Serial;
		serial_id = strstream.str();
#elif defined(LINUX)
		int fd;
		struct hd_driveid hid;
		fd = open("/dev/sda", O_RDONLY);
		if (fd < 0)
		{
			return;
		}
		if (ioctl(fd, HDIO_GET_IDENTITY, &hid) < 0)
		{
			return;
		}
		close(fd);
		serial_id = string((const char*)hid.serial_no);

#endif
#endif
	}
}
