#ifndef OBJECT3D_H
#define OBJECT3D_H
// ****************************************************************************
//  object3d.h                                                     Tao project
// ****************************************************************************
//
//   File Description:
//
//    Load a 3D model file and render it using GLC_Lib.
//
//
//
//
//
//
//
// ****************************************************************************
// This software is licensed under the GNU General Public License v3.
// See file COPYING for details.
//  (C) 1992-2010 Christophe de Dinechin <christophe@taodyne.com>
//  (C) 2010 Jerome Forissier <jerome@taodyne.com>
//  (C) 2010 Taodyne SAS
// ****************************************************************************


#include "tao/coords3d.h"
#include "tao/module_api.h"
#include <QObject>
#include <QTime>
#include <QImage>
#include <vector>
#include <iostream>
#include <GLC_World>

class LoadThread;

struct Object3D : public QObject
// ----------------------------------------------------------------------------
//   Representation of a complete 3D object
// ----------------------------------------------------------------------------
{
    // Constructor and destructor
    Object3D(kstring name = NULL, bool colored = false);
    ~Object3D();

    // Loading an objet file
    void              Load(kstring name);

    // Draw interface
    void              Draw();
    void              Identify();

    // Object bounding box
    Tao::Box3         Bounds();

    // Object3D cache
    static Object3D * Object(text name, bool colored);

    // Call ((Object3D *)arg)->Draw()
    static void       render_callback(void *arg);

public slots:
    void              percentComplete(int p);
    void              loadFinished();
    void              loadFailed();

public:
    // Debug trace
    static std::ostream& debug();

public:
    enum Status {
        NotStarted = 0,
        InProgress,
        LoadFailed,
        LoadSuccess,
    };

private:
    void              DrawObject();
    void              IdentifyObject();
    void              DrawPlaceHolder();
    void              DrawErrorPlaceHolder();

private:
    static text       toText(QString s);
    static void       initGLC();
    static void       checkCurrentContext();

private:
    typedef std::map<text, Object3D *> file_map;
    static file_map           loaded;
    static const QGLContext * context;

private:
    // Representation of an object
    GLC_World     glcWorld;
    // Thread to load file asynchronously
    LoadThread *  loadThread;
    // During load, percent complete
    int           complete;
    // Load start time
    QTime         loadTime;
    // Textured object?
    bool          hasTexture;

public:
    // File load status
    Status        status;
    // GLC_Lib error message in case status == LoadError
    QString       errorStr;
    // True if Draw should force use of current color/material
    bool          colored;

public:
    // Pointer to Tao functions
    static const Tao::ModuleApi *tao;
    // Collection of images to show load progress
#   define         NPROGRESS 21
    static QImage  progress[NPROGRESS];

    Q_OBJECT
};

#endif // OBJECT3D_H
