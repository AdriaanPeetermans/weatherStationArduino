
def parseData(d,number):
    d = d[7:len(d)]
    for i in range(number):
        nameLength = int(d[0:2])
        d = d[2:len(d)]
        name = d[0:nameLength]
        d = d[nameLength:len(d)]
        fileSize = int(d[0:10])
        d = d[10:len(d)]
        file = open(name, "w")
        file.write(str(d[0:fileSize],'utf-8'))
        d = d[fileSize:len(d)]
        
