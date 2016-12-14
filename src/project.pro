#--------------------------
#
# Project TP OpenGL DTMI 1
#
#--------------------------

CONFIG += c++11

QT += core gui opengl xml widgets
TARGET = tp_ogl_basic
TEMPLATE = app

#for QGLViewer & glm
INCLUDEPATH += ../

DESTDIR = ../bin/

QMAKE_CXXFLAGS += -DSHADERPATH=$$_PRO_FILE_PWD_

# Linux
unix:!macx {
	QMAKE_LFLAGS += -Wl,-rpath,$$_PRO_FILE_PWD_/../QGLViewer
	LIBS += -L$$_PRO_FILE_PWD_/../QGLViewer -lQGLViewer -lGLEW -lGLU
}

# macOS/X
macx {
	# installer homebrew puis glew (brew install glew)
	# installer Qt5.x
	QMAKE_LFLAGS += -Wl,-rpath,$$_PRO_FILE_PWD_/../QGLViewer
	LIBS += -L$$_PRO_FILE_PWD_/../QGLViewer -lQGLViewer -L/usr/local/lib -lGLEW
	INCLUDEPATH += /usr/local/include
}

# Windows (64b)
win32 {
	# installer VS2015 community
	# installer Qt5.x pour VS2015_x64
	# unzip glew.zip (recherche google)
	# copier lib/Release/x64/glew32.lib dans TP_OGL
	# copier bin/Release/x64/glew32.dll dans TP_OGL/bin
	# copier GL (de include) dans TP_OGL
	LIBS += -L$$_PRO_FILE_PWD_/../QGLViewer -L$$_PRO_FILE_PWD_/../ -lQGLViewer -lglew32 -lopengl32
}



SOURCES += main.cpp \
        viewer.cpp \
        Shader.cpp \
        ShaderProgram.cpp \
    bezierpatch.cpp \
    ShaderProgramBezier.cpp \
    bezierpatch_rectangle.cpp \
    bezierpatch_triangle.cpp \
    rpatch2tpatchsolver.cpp \
    productpolynom3var.cpp \
    bezierpatch_manager.cpp \
    bezierpatch_tetrahedron.cpp \
    bezierpatch_hexaedron.cpp \
    mainwindow.cpp \
    widget_patchitem.cpp \
    dialog_newpatch.cpp

HEADERS  += viewer.h \
        ShaderProgram.h \
        Shader.h \
    bezierpatch.h \
    ShaderProgramBezier.h \
    bezierpatch_rectangle.h \
    bezierpatch_triangle.h \
    rpatch2tpatchsolver.h \
    productpolynom3var.h \
    bezierpatch_manager.h \
    bezierpatch_tetrahedron.h \
    bezierpatch_hexaedron.h \
    mainwindow.h \
    widget_patchitem.h \
    dialog_newpatch.h

OTHER_FILES += \
    simpleShader.vert \
    shaderBezier.frag \
    shaderBezier.vert \
    shaderBezier.geom

FORMS += \
    mainwindow.ui \
    widget_patchitem.ui \
    dialog_newpatch.ui
