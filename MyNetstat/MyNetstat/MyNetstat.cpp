#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
using namespace std;

void retrievingTCPConnectionTable()
{
	PMIB_TCPTABLE pTcpTable;
	DWORD dwSize = 0;
	DWORD dwReturnValue = 0;
	char szLocalAddr[128];
	char szRemoteAddr[128];
	struct in_addr IpAddr;
	pTcpTable = (MIB_TCPTABLE *)malloc(sizeof(MIB_TCPTABLE));
	if (pTcpTable == NULL) {
		cout << ("Error allocating memory\n");
		return;
	}
	dwSize = sizeof(MIB_TCPTABLE);
	if ((dwReturnValue = GetTcpTable(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
		free(pTcpTable);
		pTcpTable = (MIB_TCPTABLE *)malloc(dwSize);
		if (pTcpTable == NULL) {
			printf("Error allocating memory\n");
			return;
		}
	}
	if ((dwReturnValue = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
		wprintf(L"Protocol\tState\t\tLocal Address\t\tRemote Address\n");
		for (int i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
			IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
			strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
			IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
			strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr));
			wprintf(L"TCP\t\t");
			switch (pTcpTable->table[i].dwState) {
			case MIB_TCP_STATE_CLOSED:
				wprintf(L"CLOSED\t\t");
				break;
			case MIB_TCP_STATE_LISTEN:
				wprintf(L"LISTEN\t\t");
				break;
			case MIB_TCP_STATE_SYN_SENT:
				wprintf(L"SYN-SENT\t");
				break;
			case MIB_TCP_STATE_SYN_RCVD:
				wprintf(L"SYN-RECEIVED\t");
				break;
			case MIB_TCP_STATE_ESTAB:
				wprintf(L"ESTABLISHED\t");
				break;
			case MIB_TCP_STATE_FIN_WAIT1:
				wprintf(L"FIN-WAIT-1\t");
				break;
			case MIB_TCP_STATE_FIN_WAIT2:
				wprintf(L"FIN-WAIT-2\t\t");
				break;
			case MIB_TCP_STATE_CLOSE_WAIT:
				wprintf(L"CLOSE-WAIT\t");
				break;
			case MIB_TCP_STATE_CLOSING:
				wprintf(L"CLOSING\t\t");
				break;
			case MIB_TCP_STATE_LAST_ACK:
				wprintf(L"LAST-ACK\t");
				break;
			case MIB_TCP_STATE_TIME_WAIT:
				wprintf(L"TIME-WAIT\t");
				break;
			case MIB_TCP_STATE_DELETE_TCB:
				wprintf(L"DELETE-TCB\t");
				break;
			default:
				wprintf(L"UNKNOWN dwState value");
				break;
			}
			if (strlen(szLocalAddr) > 7) {
				wprintf(L"%hs : %u\t", szLocalAddr, ntohs((u_short)pTcpTable->table[i].dwLocalPort));
			}
			else {
				wprintf(L"%hs : %u\t\t", szLocalAddr, ntohs((u_short)pTcpTable->table[i].dwLocalPort));
			}
			wprintf(L"%hs : %u", szRemoteAddr, ntohs((u_short)pTcpTable->table[i].dwRemotePort));
			wprintf(L"\n");
		}
	}
	else {
		printf("\tGetTcpTable failed with %d\n", dwReturnValue);
		free(pTcpTable);
		return;
	}
	if (pTcpTable != NULL) {
		free(pTcpTable);
		pTcpTable = NULL;
	}
}

void retrievingUDPListenerTable()
{
	PMIB_UDPTABLE pUdpTable;
	DWORD dwSize = 0;
	DWORD dwReturnValue = 0;
	char szLocalAddr[128];
	struct in_addr IpAddr;
	pUdpTable = (MIB_UDPTABLE *)malloc(sizeof(MIB_UDPTABLE));
	if (pUdpTable == NULL) {
		printf("Error allocating memory\n");
		return;
	}
	dwSize = sizeof(MIB_UDPTABLE);
	if ((dwReturnValue = GetUdpTable(pUdpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
		free(pUdpTable);
		pUdpTable = (MIB_UDPTABLE *)malloc(dwSize);
		if (pUdpTable == NULL) {
			printf("Error allocating memory\n");
			return;
		}
	}
	if ((dwReturnValue = GetUdpTable(pUdpTable, &dwSize, TRUE)) == NO_ERROR)
	{
		wprintf(L"Protocol\tLocalAddress\n");
		for (int i = 0; i < (int)pUdpTable->dwNumEntries; i++) {
			wprintf(L"UDP\t");
			IpAddr.S_un.S_addr = (u_long)pUdpTable->table[i].dwLocalAddr;
			strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));
			wprintf(L"\t%hs : %u\n", szLocalAddr, ntohs((u_short)pUdpTable->table[i].dwLocalPort));
		}
	}
}

void retrievingIpStatictics(ULONG family)
{
	DWORD dwReturnValue = 0;
	PMIB_IPSTATS ipStatictics;
	ipStatictics = (MIB_IPSTATS *)malloc(sizeof(MIB_IPSTATS));
	if (ipStatictics == NULL) {
		printf("Error allocating memory");
		return;
	}
	dwReturnValue = GetIpStatisticsEx(ipStatictics, family);
	if (dwReturnValue != NO_ERROR) {
		printf("GetIpStatistics call failed with %d\n", dwReturnValue);
	}
	else {
		printf("\tIP forwarding: \t\t");
		switch (ipStatictics->dwForwarding) {
		case MIB_IP_FORWARDING:
			printf("Enabled\n");
			break;
		case MIB_IP_NOT_FORWARDING:
			printf("Disabled\n");
			break;
		default:
			printf("unknown value = %d\n", ipStatictics->dwForwarding);
			break;
		}
		wprintf(L"\tDefault initial TTL: \t\t\t\t\t\t\t%u\n", ipStatictics->dwDefaultTTL);
		wprintf(L"\tNumber of received datagrams: \t\t\t\t\t\t%u\n", ipStatictics->dwInReceives);
		wprintf(L"\tNumber of received datagrams with header errors: \t\t\t%u\n", ipStatictics->dwInHdrErrors);
		wprintf(L"\tNumber of received datagrams with address errors: \t\t\t%u\n", ipStatictics->dwInAddrErrors);
		wprintf(L"\tNumber of datagrams forwarded: \t\t\t\t\t\t%ld\n", ipStatictics->dwForwDatagrams);
		wprintf(L"\tNumber of received datagrams with an unknown protocol: \t\t\t%u\n", ipStatictics->dwInUnknownProtos);
		wprintf(L"\tNumber of received datagrams discarded: \t\t\t\t%u\n", ipStatictics->dwInDiscards);
		wprintf(L"\tNumber of received datagrams delivered: \t\t\t\t%u\n", ipStatictics->dwInDelivers);
		wprintf(L"\tNumber of outgoing datagrams requested to transmit: \t\t\t%u\n", ipStatictics->dwOutRequests);
		wprintf(L"\tNumber of outgoing datagrams discarded for routing: \t\t\t%u\n", ipStatictics->dwRoutingDiscards);
		wprintf(L"\tNumber of outgoing datagrams discarded: \t\t\t\t%u\n", ipStatictics->dwOutDiscards);
		wprintf(L"\tNumber of outgoing datagrams with no route to destination discarded: \t%u\n", ipStatictics->dwOutNoRoutes);
		wprintf(L"\tFragment reassembly timeout: \t\t\t\t\t\t%u\n", ipStatictics->dwReasmTimeout);
		wprintf(L"\tNumber of datagrams that required reassembly: \t\t\t\t%u\n", ipStatictics->dwReasmReqds);
		wprintf(L"\tNumber of datagrams successfully reassembled: \t\t\t\t%u\n", ipStatictics->dwReasmOks);
		wprintf(L"\tNumber of datagrams that could not be reassembled: \t\t\t%u\n", ipStatictics->dwReasmFails);
		wprintf(L"\tNumber of datagrams fragmented successfully: \t\t\t\t%u\n", ipStatictics->dwFragOks);
		wprintf(L"\tNumber of datagrams not fragmented and discarded: \t\t\t%u\n", ipStatictics->dwFragFails);
		wprintf(L"\tNumber of fragments created: \t\t\t\t\t\t%u\n", ipStatictics->dwFragCreates);
		wprintf(L"\tNumber of interfaces: \t\t\t\t\t\t\t%u\n", ipStatictics->dwNumIf);
		wprintf(L"\tNumber of IP addresses: \t\t\t\t\t\t%u\n", ipStatictics->dwNumAddr);
		wprintf(L"\tNumber of routes: \t\t\t\t\t\t\t%u\n", ipStatictics->dwNumRoutes);
	}
	if (ipStatictics) {
		free(ipStatictics);
	}
}

void retrievingIcmpStatictics(ULONG family)
{
	DWORD dwReturnValue = 0;
	PMIB_ICMP_EX icmpStatictics;
	icmpStatictics = (MIB_ICMP_EX *)malloc(sizeof(MIB_ICMP_EX));
	if (icmpStatictics == NULL) {
		wprintf(L"Error allocating memory");
		return;
	}
	dwReturnValue = GetIcmpStatisticsEx(icmpStatictics, family);
	if (dwReturnValue != NO_ERROR) {
		wprintf(L"GetIcmpStatistics call failed with %d\n", dwReturnValue);
	}
	else {
		wprintf(L"\tNumber of incoming ICMP messages: %ld\n", icmpStatictics->icmpInStats.dwMsgs);
		wprintf(L"\tNumber of incoming ICMP errors received: %ld\n", icmpStatictics->icmpInStats.dwErrors);
		wprintf(L"\tNumber of outgoing ICMP messages: %ld\n", icmpStatictics->icmpOutStats.dwMsgs);
		wprintf(L"\tNumber of outgoing ICMP errors received: %ld\n", icmpStatictics->icmpOutStats.dwErrors);
	}
	if (icmpStatictics) {
		free(icmpStatictics);
	}
}

void retrievingTcpStatictics(ULONG family)
{
	DWORD dwReturnValue = 0;
	PMIB_TCPSTATS tcpStatictics;
	tcpStatictics = (MIB_TCPSTATS *)malloc(sizeof(MIB_IPSTATS));
	if (tcpStatictics == NULL) {
		wprintf(L"Error allocating memory");
		return;
	}
	dwReturnValue = GetTcpStatisticsEx(tcpStatictics, family);
	if (dwReturnValue != NO_ERROR) {
		wprintf(L"GetTcpStatistics call failed with %d\n", dwReturnValue);
	}
	else {
		wprintf(L"\tActive Opens: %ld\n", tcpStatictics->dwActiveOpens);
		wprintf(L"\tPassive Opens: %ld\n", tcpStatictics->dwPassiveOpens);
		wprintf(L"\tSegments Recv: %ld\n", tcpStatictics->dwInSegs);
		wprintf(L"\tSegments Xmit: %ld\n", tcpStatictics->dwOutSegs);
		wprintf(L"\tTotal # Conxs: %ld\n", tcpStatictics->dwNumConns);
	}
	if (tcpStatictics) {
		free(tcpStatictics);
	}
}

void retrievingUdpStatictics(ULONG family)
{
	DWORD dwReturnValue = 0;
	PMIB_UDPSTATS udpStatictics;
	udpStatictics = (MIB_UDPSTATS *)malloc(sizeof(MIB_UDPSTATS));
	if (udpStatictics == NULL) {
		wprintf(L"Error allocating memory");
		return;
	}
	dwReturnValue = GetUdpStatisticsEx(udpStatictics, family);
	if (dwReturnValue != NO_ERROR) {
		wprintf(L"GetUdpStatistics call failed with %d\n", dwReturnValue);
	}
	else {
		printf("\tNumber of datagrams received: %ld\n", udpStatictics->dwInDatagrams);
		printf("\tNumber of datagrams discarded because the port number was bad: %ld\n", udpStatictics->dwNoPorts);
		printf("\tNumber of erroneous datagrams received: %ld\n", udpStatictics->dwInErrors);
		printf("\tNumber of datagrams transmitted: %ld\n", udpStatictics->dwOutDatagrams);
	}
	if (udpStatictics) {
		free(udpStatictics);
	}
}

void showIpStatictics()
{
	wprintf(L"\nIP Statictics for IPv4: \n");
	retrievingIpStatictics(AF_INET);
	wprintf(L"\nIP Statictics for IPv6: \n");
	retrievingIpStatictics(AF_INET6);
}

void showIcmpStatictics()
{
	wprintf(L"\nICMP Statictics for IPv4: \n");
	retrievingIcmpStatictics(AF_INET);
	wprintf(L"\nICMP Statictics for IPv6: \n");
	retrievingIcmpStatictics(AF_INET6);
}

void showTcpStatictics()
{
	wprintf(L"\nTCP Statictics for IPv4: \n");
	retrievingTcpStatictics(AF_INET);
	wprintf(L"\nTCP Statictics for IPv6: \n");
	retrievingTcpStatictics(AF_INET6);
}

void showUdpStatictics()
{
	wprintf(L"\nUDP Statictics for IPv4: \n");
	retrievingUdpStatictics(AF_INET);
	wprintf(L"\nUDP Statictics for IPv6: \n");
	retrievingUdpStatictics(AF_INET6);
}

int main()
{
	char commandLine[20];
	while(true) {
		printf("Enter your command (Press q to exit): ");
		scanf("%[^\n]%*c", commandLine);

		if (strcmp("q", commandLine) == 0 || strcmp("Q", commandLine) == 0) {
			break;
		}
		if (strcmp("my_netstat", commandLine) == 0 || strcmp("my_netstat -p tcp -a", commandLine) == 0) {
			retrievingTCPConnectionTable();
		}
		else if (strcmp("my_netstat -a", commandLine) == 0) {
			retrievingTCPConnectionTable();
			retrievingUDPListenerTable();
		}
		else if (strcmp("my_netstat -p udp -a", commandLine) == 0) {
			retrievingUDPListenerTable();
		}
		else if (strcmp("my_netstat -s", commandLine) == 0)
		{
			showIpStatictics();
			showIcmpStatictics();
			showTcpStatictics();
			showUdpStatictics();
		}
		else {
			printf("\nPlease enter the valid command line!\n");
		}
	}
	system("pause");
}

