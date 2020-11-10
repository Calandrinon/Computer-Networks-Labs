import subprocess

# A cool idea that I came up with when I tried to obtain my own local IP
# to avoid letting a program send broadcasted messages to itself

def get_local_ip_address():
    # This makes use of the ip addr command to obtain the local ip
    local_ip = subprocess.check_output("ip addr | grep 192.168 | awk '{print $2}' | sed 's/\(.*\)\\/24/\\1/g'", shell=True, universal_newlines=True).strip()
    return local_ip

print(get_local_ip_address())
