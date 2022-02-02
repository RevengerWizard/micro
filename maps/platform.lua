return {
  version = "1.5",
  luaversion = "5.1",
  tiledversion = "1.7.2",
  orientation = "orthogonal",
  renderorder = "right-down",
  width = 24,
  height = 25,
  tilewidth = 8,
  tileheight = 8,
  nextlayerid = 12,
  nextobjectid = 57,
  backgroundcolor = { 41, 30, 49 },
  properties = {},
  tilesets = {
    {
      name = "platformer",
      firstgid = 1,
      tilewidth = 8,
      tileheight = 8,
      spacing = 0,
      margin = 0,
      columns = 12,
      image = "../assets/tilesets/platformer.png",
      imagewidth = 96,
      imageheight = 64,
      objectalignment = "unspecified",
      tileoffset = {
        x = 0,
        y = 0
      },
      grid = {
        orientation = "orthogonal",
        width = 8,
        height = 8
      },
      properties = {},
      wangsets = {},
      tilecount = 96,
      tiles = {
        {
          id = 24,
          properties = {
            ["climb"] = true,
            ["top"] = true
          }
        },
        {
          id = 36,
          properties = {
            ["climb"] = true
          }
        },
        {
          id = 48,
          properties = {
            ["climb"] = true
          }
        }
      }
    }
  },
  layers = {
    {
      type = "tilelayer",
      x = 0,
      y = 0,
      width = 24,
      height = 25,
      id = 1,
      name = "tilemap",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {},
      encoding = "lua",
      data = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 52, 0, 25, 0, 0, 0, 0, 52, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 29, 3, 4, 49, 4, 2147483677, 0, 0, 3, 4, 4, 3, 2147483677, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 53, 4, 49, 28, 0, 0, 0, 38, 16, 0, 28, 0, 0, 0, 0, 40, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 65, 49, 0, 0, 0, 0, 39, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 50, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 3, 49, 2, 4, 2147483677, 0, 0,
        0, 0, 0, 0, 0, 2, 4, 4, 4, 13, 14, 14, 14, 14, 15, 2, 2, 16, 49, 18, 3, 56, 0, 0,
        0, 0, 0, 0, 2, 3, 54, 26, 27, 0, 0, 0, 0, 0, 0, 17, 18, 6, 49, 3, 3, 44, 0, 0,
        0, 0, 55, 2, 3, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 38, 49, 65, 54, 0, 0, 0,
        0, 0, 0, 66, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 49, 0, 66, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 0, 41, 2147483689, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 52, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0,
        0, 0, 50, 4, 13, 14, 0, 0, 14, 15, 2, 3, 3, 5, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0,
        0, 0, 29, 4, 0, 0, 0, 0, 0, 0, 28, 0, 66, 38, 0, 0, 0, 29, 4, 56, 0, 0, 0, 0,
        0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0,
        0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
      }
    },
    {
      type = "objectgroup",
      draworder = "topdown",
      id = 2,
      name = "collisions",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {
        ["collidable"] = true
      },
      objects = {
        {
          id = 1,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 80,
          y = 144,
          width = 32,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 2,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 24,
          y = 144,
          width = 8,
          height = 16,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 3,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 16,
          y = 152,
          width = 8,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 13,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 32,
          y = 40,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 14,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 40,
          y = 48,
          width = 8,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 15,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 56,
          y = 40,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 16,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 88,
          y = 40,
          width = 40,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 17,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 96,
          y = 48,
          width = 8,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 18,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 120,
          y = 88,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 19,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 136,
          y = 80,
          width = 8,
          height = 24,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 24,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 152,
          y = 80,
          width = 16,
          height = 24,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 26,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 160,
          y = 128,
          width = 16,
          height = 3,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 27,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 152,
          y = 48,
          width = 8,
          height = 3,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 28,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 136,
          y = 152,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 29,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 168,
          y = 80,
          width = 8,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 31,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 40,
          y = 88,
          width = 32,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 32,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 32,
          y = 96,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 33,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 24,
          y = 104,
          width = 16,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 37,
          name = "",
          type = "solid",
          shape = "rectangle",
          x = 24,
          y = 40,
          width = 8,
          height = 8,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        }
      }
    },
    {
      type = "objectgroup",
      draworder = "topdown",
      id = 7,
      name = "stairs",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {},
      objects = {
        {
          id = 51,
          name = "",
          type = "stair",
          shape = "rectangle",
          x = 48,
          y = 39,
          width = 8,
          height = 49,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        },
        {
          id = 52,
          name = "",
          type = "stair",
          shape = "rectangle",
          x = 144,
          y = 79,
          width = 8,
          height = 73,
          rotation = 0,
          visible = true,
          properties = {
            ["collidable"] = true
          }
        }
      }
    },
    {
      type = "objectgroup",
      draworder = "topdown",
      id = 3,
      name = "platforms",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {
        ["collidable"] = true
      },
      objects = {
        {
          id = 34,
          name = "",
          type = "platform",
          shape = "rectangle",
          x = 72,
          y = 88,
          width = 48,
          height = 3,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 35,
          name = "",
          type = "platform",
          shape = "rectangle",
          x = 64,
          y = 144,
          width = 16,
          height = 3,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 36,
          name = "",
          type = "platform",
          shape = "rectangle",
          x = 32,
          y = 144,
          width = 16,
          height = 3,
          rotation = 0,
          visible = true,
          properties = {}
        }
      }
    },
    {
      type = "objectgroup",
      draworder = "topdown",
      id = 6,
      name = "entities",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {},
      objects = {
        {
          id = 43,
          name = "potion_red",
          type = "potion",
          shape = "rectangle",
          x = 154,
          y = 40,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 44,
          name = "dwarf_pickaxe",
          type = "dwarf",
          shape = "rectangle",
          x = 94,
          y = 30,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 45,
          name = "key_gold",
          type = "key",
          shape = "rectangle",
          x = 166,
          y = 118,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 46,
          name = "dwarf",
          type = "dwarf",
          shape = "rectangle",
          x = 98,
          y = 134,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 47,
          name = "chest2",
          type = "chest",
          shape = "rectangle",
          x = 26,
          y = 134,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 49,
          name = "barbarian",
          type = "barbarian",
          shape = "rectangle",
          x = 66,
          y = 78,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 50,
          name = "small_demon",
          type = "small_demon",
          shape = "rectangle",
          x = 90,
          y = 78,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        },
        {
          id = 54,
          name = "oldman",
          type = "oldman",
          shape = "rectangle",
          x = 166,
          y = 70,
          width = 4,
          height = 4,
          rotation = 0,
          visible = true,
          properties = {}
        }
      }
    }
  }
}
