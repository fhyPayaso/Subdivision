cmake_minimum_required(VERSION 3.15)
project(Subdivision)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(lib)
include_directories(lib/arcball)

add_executable(Subdivision
        lib/arcball/Arcball.cpp
        lib/arcball/Arcball.h
        lib/arcball/Camera.cpp
        lib/arcball/Camera.h
        lib/arcball/Matrix44.cpp
        lib/arcball/Matrix44.h
        lib/arcball/Quaternion.cpp
        lib/arcball/Quaternion.h
        lib/arcball/Vector3d.cpp
        lib/arcball/Vector3d.h
        lib/arcball/Viewport.cpp
        lib/arcball/Viewport.h
        bitmap.cpp
        bitmap.h
        ChildFrm.cpp
        ChildFrm.h
        DC.cpp
        DC.h
        DibSection.cpp
        DibSection.h
        MainFrm.cpp
        MainFrm.h
        Mesh3D.cpp
        Mesh3D.h
        MeshSubdivision.cpp
        MeshSubdivision.h
        resource.h
        stdafx.cpp
        stdafx.h
        Subdivision.cpp
        Subdivision.h
        SubdivisionDoc.cpp
        SubdivisionDoc.h
        SubdivisionView.cpp
        SubdivisionView.h)
