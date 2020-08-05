
--
-- core libraries
--

   -- base library
   project "base"
      files {
         "../../include/mixr/base/**.h*",
         "../../include/mixr/base/**.inl",
         "../../include/mixr/base/**.epp",
         "../../include/mixr/base/osg/*",
         "../../src/base/**.h*",
         "../../src/base/**.cpp",
         "../../src/base/**.y",
         "../../src/base/**.l"
      }
      excludes {
         "../../src/base/osg/Matrix_implementation.cpp",
         "../../src/base/util/platform/system_linux.cpp",
         "../../src/base/util/platform/system_mingw.cpp",
         "../../src/base/threads/platform/*_linux.cpp"
      }
      includedirs { MIXR_IncPath }
      targetname "base"

   -- OpenGL-based graphics library
   project "graphics"
      files {
         "../../include/mixr/graphics/**.h*",
         "../../src/graphics/**.h*",
         "../../src/graphics/**.cpp",
         "../../src/graphics/**.l"
      }
      includedirs { MIXR_IncPath, MIXR_3rdPartyIncPath }
      includedirs { MIXR_3rdPartyIncPath.."/freetype2" }
      defines { "FTGL_LIBRARY_STATIC" }
      targetname "graphics"

   -- DAFIF airport loader library
   project "dafif"
      files {
         "../../include/mixr/dafif/**.h*",
         "../../src/dafif/**.cpp"
      }
      includedirs { MIXR_IncPath }
      targetname "dafif"

   -- graphical instruments library
   project "instruments"
      files {
         "../../include/mixr/instruments/**.h*",
         "../../include/mixr/instruments/**.epp",
         "../../src/instruments/**.cpp"
      }
      includedirs { MIXR_IncPath }
      targetname "instruments"

   -- linear systems library
   project "linearsystem"
      files {
         "../../include/mixr/linearsystem/**.h*",
         "../../src/linearsystem/**.cpp"
      }
      includedirs { MIXR_IncPath }
      targetname "linearsystem"

   -- linkage library
   project "linkage"
      files {
         "../../include/mixr/linkage/**.h*",
         "../../src/linkage/**.*",
         "../../src/linkage/**.h*"
      }
      excludes { "../../src/linkage/platform/UsbJoystick_linux.*"   }
      includedirs { MIXR_IncPath }
      targetname "linkage"

   -- models library
   project "models"
      files {
         "../../include/mixr/models/**.h*",
         "../../include/mixr/models/**.inl",
         "../../src/models/**.cpp"
      }
      includedirs { MIXR_IncPath, MIXR_3rdPartyIncPath }
      includedirs { MIXR_3rdPartyIncPath.."/JSBSim" }
      targetname "models"

   -- data recorder library
   project "recorder"
      files {
         "../../include/mixr/recorder/**.h*",
         "../../include/mixr/recorder/*.inl",
         "../../include/mixr/recorder/**.proto",
         "../../src/recorder/**.cpp",
         "../../src/recorder/**.cc"
      }
      defines { "_SCL_SECURE_NO_WARNINGS" } -- suppress protocol buffer warning
      includedirs { MIXR_IncPath, MIXR_3rdPartyIncPath }
      targetname "recorder"

   -- simulation library
   project "simulation"
      files {
         "../../include/mixr/simulation/**.h*",
         "../../include/mixr/simulation/**.inl",
         "../../src/simulation/**.h*",
         "../../src/simulation/**.cpp"
      }
      includedirs { MIXR_IncPath, MIXR_3rdPartyIncPath }
      targetname "simulation"

   -- terrain library
   project "terrain"
      files {
         "../../include/mixr/terrain/**.h*",
         "../../src/terrain/**.cpp"
      }
      includedirs { MIXR_IncPath }
      targetname "terrain"

   -- OpenGL GLUT interface library
   project "ui_glut"
      files {
         "../../include/mixr/ui/glut/**.h*",
         "../../src/ui/glut/**.cpp"
      }
      includedirs { MIXR_IncPath, MIXR_3rdPartyIncPath }
      targetname "ui_glut"
