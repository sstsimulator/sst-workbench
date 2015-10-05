////////////////////////////////////////////////////////////////////////
// Copyright 2009-2014 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2014, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
////////////////////////////////////////////////////////////////////////

#ifndef PYTHONEXPORTER_H
#define PYTHONEXPORTER_H

#include "GlobalIncludes.h"

#include "WiringScene.h"

class WiringScene;

#define TAB       "    "
#define SEPERATOR out<<"##############################################################################"<<endl
#define HEADER    out<<"########################################"<<endl
#define BLANKLINE out<<endl

///////////////////////////////////////////////////////////////////////////////

class PythonExporter
{
public:
    PythonExporter(WiringScene* Scene, QString FilePath);
    ~PythonExporter();

    bool PerformExportOfPythonFile();

private:
    bool CheckSSTStartupConfigComponent();
    bool CheckComponentProperties();
    bool CheckComponentUnconnectedPorts();
    bool CheckUnconnectedWires();
    void DisplayExportStatus();
    bool CreatePythonFile();

    void WriteHeader(QTextStream& out);
    void WriteSSTStartupProgramOptions(QTextStream& out);
    void WriteComponents(QTextStream& out);
    void WriteLinks(QTextStream& out);
    void WriteFooter(QTextStream& out);

    QString GetComponentVarName(QString CompUniqueName);

private:
    WiringScene*          m_Scene;
    QString               m_FilePath;
    GraphicItemComponent* m_StartupConfigComponent;
    bool                  m_bFinalResult;
    QStringList           m_ExportErrorsList;
    QStringList           m_ExportWarningsList;
};

#endif // PYTHONEXPORTER_H
