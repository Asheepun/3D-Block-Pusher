import array

fileName = "bunny"

objFile = open(fileName + ".obj", "r")

lines = objFile.read().split("\n")

vertices = [[0.0, 0.0, 0.0]]
textureVertices = [[0.0, 0.0]]
normalVertices = [[0.0, 0.0, 0.0]]
faces = []

for i in range(0, len(lines)):

    words = lines[i].split(" ")

    words = list(filter(lambda word: not (word == ""), words))

    if(words == []):
        continue

    if(words[0] == "v"):
        vertices.append([float(words[1]), float(words[2]), float(words[3])]);

    if(words[0] == "vn"):
        normalVertices.append([float(words[1]), float(words[2]), float(words[3])]);

    if(words[0] == "vt"):
        textureVertices.append([float(words[1]), float(words[2])]);

    if(words[0] == "f"):

        face = []

        for index in words[1].split("/") + words[2].split("/") + words[3].split("/"):
            if(index == ""):
                index = 0
            face.append(int(index))

        faces.append(face)

        if(len(words) == 5):
            face2 = []

            for index in words[1].split("/") + words[3].split("/") + words[4].split("/"):
                if(index == ""):
                    index = 0
                face2.append(int(index))

            faces.append(face2)

#generate mesh
mesh = []

for face in faces:

    for i in range(0, 3):
        mesh += vertices[face[i * 3]]
        mesh += textureVertices[face[i * 3 + 1]]
        mesh += normalVertices[face[i * 3 + 2]]

#write mesh to file as binary float array
floatArray = array.array("f", mesh)
outputFile = open(fileName + ".mesh", "wb")
floatArray.tofile(outputFile)
outputFile.close()
