import sys
import shodan
import ctypes

SHODAN_API_KEY  = ''
CISCO_BANNER    = 'Server: cisco-IOS'
SEARCH_LIMIT    = 20

def scan_nmap(ip_addresses):
    for ip in ip_addresses:
        if os.access("%s.txt" %(ip), os.F_OK):
            continue

        print "Scanning %s.." %(ip)
        scan_command = "nmap -sV {IP} > {IP}.txt".format(IP=ip)
        os.system(scan_command)
`
def main():
    shodan_api = shodan.Shodan(SHODAN_API_KEY)
    
    try:
        results = shodan_api.search(CISCO_BANNER, limit=SEARCH_LIMIT)
        scan_nmap(results['ip_str'])
    except shodan.APIError, e:
        print "Error querying shodan for the specified banner(%s). Exception = %s" %(CISCO_BANNER, e)
        return 

    ctypes.windll.user32.MessageBoxA(0, "Done Scanning", "Done Scanning", 0,0)
if __name__ == '__main__':
    sys.exit(main())