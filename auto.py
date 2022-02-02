# Aseprite doesn't have an advanced JSON export option
# and I don't like the JSON file generated

import os
import json

dir = "data/characters/"

def regenerate(dir, file):
    fileName = dir + file

    input = {}

    with open(fileName, "r") as read_file:
        input = json.load(read_file)

    output = {
        "frames": 
        [
            dict() for x in range(len(input["frames"]))
        ],
        "meta": 
        {
            "name": file.split(".")[0],
            "image": "assets/" + file.split(".")[0] + ".png",
            "size": 
            {
                "w": input["meta"]["size"]["w"],
                "h": input["meta"]["size"]["h"],
            },
            "frameTags": 
            [
                dict() for x in range(len(input["meta"]["frameTags"]))
            ]
        }
    }

    for i in range(len(output["frames"])):
        output["frames"][i] = {
            "frame":
            {
                "x": input["frames"][i]["frame"]["x"],
                "y": input["frames"][i]["frame"]["y"],
                "w": input["frames"][i]["frame"]["w"],
                "h": input["frames"][i]["frame"]["h"],
                "duration": input["frames"][i]["duration"],
                "ox": 0,
                "oy": 0,
                #"event": False
            }
        }

    for j in range(len(output["meta"]["frameTags"])):
        output["meta"]["frameTags"][j] = {
            "name": input["meta"]["frameTags"][j]["name"], 
            "from": input["meta"]["frameTags"][j]["from"], 
            "to": input["meta"]["frameTags"][j]["to"], 
            "direction": input["meta"]["frameTags"][j]["direction"] 
        }

    with open(fileName, "w") as write_file:
        json.dump(output, write_file, indent = 4)


def regenerateDir(dir):
    listArr = []

    for file in os.listdir(dir):
        if file.endswith('.json'):
            listArr.append(file)
        else:
            #print(file)
            continue

    for l in listArr:
        # regenerate each JSON file
        regenerate(dir, l)
        print(l)

def regenerateFile(f):
    for file in os.listdir(dir):
        if file.endswith('.json') and file == f:
            print(file)
            regenerate(dir, file)
            #return
        else:
            continue

    print(dir)

#regenerateDir(dir)
regenerateFile('dwarf_pickaxe.json')