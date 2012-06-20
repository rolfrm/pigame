import os
import pickle
endings = [".c",".cpp"]
includes = "-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads"
link = "-L/opt/vc/lib/  -lEGL -lGLESv2 -lbcm_host -lvcos"
output = "start"
try:
    with open("pycache","r") as fptr:
        pycache = pickle.load(fptr)
except:
    pycache = {}




def get_files_to_compile():
    objs = os.listdir("./")
    compfiles = []
    compobjs = []
    for obj in objs:
        if obj[0] == ".":
            continue
        for end in endings:
            
            lend = len(end)
            endsyms = obj[-lend:]
            if endsyms == end:
                compfiles.append(obj)
                compobjs.append(obj[:-lend] + ".o")
    return compfiles,compobjs
ftc,ftl = get_files_to_compile()




for i in ftc:
    chtime = os.path.getmtime(i)
    print chtime
    if i not in pycache.keys() or pycache[i] < chtime:
        
        if False and i[-2:] == ".c":
            cc = "gcc"
        else:
            cc = "g++"
        call = "{2} -c {0} {1} -O3 ".format(i,includes,cc)
        print call
        pycache[i] = os.path.getmtime(i)
        os.system(call)
    else:
        print "skipping",i

with open("pycache","w") as fptr:
    pickle.dump(pycache,fptr)

acc_link = "g++ {0} -o {1} -O3 ".format(link,output)
for i in ftl:
    acc_link +=i + " "
print acc_link
os.system(acc_link)
