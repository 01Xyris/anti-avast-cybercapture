# anti-avast-cybercapture
Detection for Avast CyberCapture sandbox environment through GetTokenInformation

1. `OpenProcessToken()` grabs a handle to our process's security token
2. `GetTokenInformation()` pulls the list of all privileges 
3. `LookupPrivilegeNameW()` converts each privilege from a number to a readable name
4. Check if `SeShutdownPrivilege` and `SeUndockPrivilege` exist - if not, we're sandboxed


<img width="789" height="442" alt="sandbox" src="https://github.com/user-attachments/assets/f1b721be-3468-4aec-8ec5-d4a0ab760aeb" />
<img width="738" height="382" alt="nosandbox" src="https://github.com/user-attachments/assets/d3a11b70-bb78-45a6-89a8-9752f8e27a22" />


![wtf_50](https://github.com/user-attachments/assets/51b85c30-0ec1-4bee-935b-c5134473fd88)

the high cpu usage for calculation is a bad idea but it's whatever.

