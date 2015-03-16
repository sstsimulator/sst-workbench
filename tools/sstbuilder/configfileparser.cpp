// Copyright 2009-2015 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2015, Sandia Corporation
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include "configfileparser.h"

#define SSTINIFILENAME "/.sstbuilder"

ConfigFileParser::ConfigFileParser(QObject *parent) :
    QObject(parent)
{
    // Init variables
    m_sstConfigFilePath = "";
    m_HeaderNameList.clear();
    m_HeaderTypeList.clear();
    m_ParsingErrorList.clear();

    // Get the headers and rules
    initializeHeaderNamesAndRules();
}

void ConfigFileParser::setConfigFilePath(QString& filePath)
{
   m_sstConfigFilePath = filePath;
}

int ConfigFileParser::getNumberOfHeaders()
{
    return m_HeaderDataList.count();
}

HeaderData* ConfigFileParser::getHeader(int index)
{
    return m_HeaderDataList[index];
}

void ConfigFileParser::parseConfigFile()
{
    QFile configFile;
    int x;

    // Try to open the file
    if (!m_sstConfigFilePath.isEmpty()) {

        // Set the file name
        configFile.setFileName(m_sstConfigFilePath);

        // Try to open the file
        if (!configFile.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(NULL, tr("Error"), tr("Could not open file"));
            return;
        }

        // Now populate the Header identified by the Headername  for the headers
        for (x = 0; x < m_HeaderNameList.count(); x++) {
            matchAndPopulateHeaderAndItsEntries(m_HeaderNameList[x], m_HeaderTypeList[x], configFile);
        }

        // Close the file
        configFile.close();
    }
}

void ConfigFileParser::initializeHeaderNamesAndRules()
{
    m_HeaderNameList = m_PersistantData.GetConfigFileHeaderNameList();
    m_HeaderTypeList = m_PersistantData.GetConfigFileHeaderTypeList();
}

void ConfigFileParser::matchAndPopulateHeaderAndItsEntries(QString& HeaderName, QString& HeaderType, QFile& File)
{
    QString     testLine;
    QString     errormsg;
    HeaderData* newHeader;

    // This method searches the file for a specific HeaderName.  If it finds it,
    // it adds it to the database and then populates the entries under the header
    // into a HeaderBlock object.

    // The file is open, setup a stream to it
    QTextStream configFileStream(&File);

    // Always search for the HeaderName from the start of the file.
    configFileStream.seek(0);
    m_currentFilelinecount = 0;
    m_nextFilelinecount = 0;

    // Read each line in the file and try to find the header
    while(!configFileStream.atEnd()) {
        testLine = configFileStream.readLine();
        m_currentFilelinecount++;

        // Is the line the same as the header?
        if (testLine == HeaderName) {
            newHeader = (HeaderData*)new HeaderData(HeaderName, HeaderType);
            if (0 != newHeader) {
                m_HeaderDataList.append(newHeader);

                // We found the Header in the file, now poulate the HeaderBlock
                populateHeaderBlock(configFileStream, newHeader, m_currentFilelinecount);
            } else {
                errormsg = tr("ERROR: Could not create HeaderData structure for Header '") + HeaderName + tr("' in configure file.  Are you out of memory?");
                m_ParsingErrorList += errormsg;
            }
            // We found the header we were searching for, we dont need to look anymore
            return;
        }
    }

    errormsg = tr("ERROR: Did not find Header '") + HeaderName + tr("' in configure file");
    m_ParsingErrorList += errormsg;
}

void ConfigFileParser::populateHeaderBlock(QTextStream& Stream, HeaderData* pHeaderData, int HeaderLine)
{
//    QString    dbgmsg = "";
    QString    testLine = "";
    QString    errormsg = "";
    QString    EntryName = "";
    QString    VarName = "";
    QString    InfoText = "";
    int        startIndex = -1;
    int        endIndex = -1;
    EntryData* newEntry;
    QChar      endIndexChar;

    // The HeaderLine, indicates where we currently are in the file when we found the header
    m_currentFilelinecount = HeaderLine;
    m_nextFilelinecount = HeaderLine;

    // Get the Initial line that we assign to the testLine used in the while loop
    // NOTE: nextLine may be advanced several lines as we parse the file
    m_nextLine = Stream.readLine();
    m_nextFilelinecount++;

    // Look at all lines from the start of the header to the end of the header (or EOF)
    while (!Stream.atEnd()) {
        testLine = m_nextLine;
        m_currentFilelinecount = m_nextFilelinecount;

        // Look for "_ACEOF" that indicates the end of this header section (meaning we are done)
        startIndex = testLine.indexOf("_ACEOF", 0);
        if (-1 != startIndex) {
            return;
        }

        // Look for ":" with no space after that indicates the end of this header section (meaning we are done)
        startIndex = testLine.indexOf(":", 0);
        if (-1 != startIndex) {
            if (QChar(' ') != testLine[startIndex+1]) {
                return;
            }
        }

        // Figure out if we have to handle a Special Header Block ("Some influential environment variables:")
        if ("Special" != pHeaderData->getHeaderType()) {
            // HANDLE NORMAL TYPE HEADER BLOCK
            // Look for "  --" which is the indicator of an entry at the start of the line.  If found, then we dig deeper to start
            // decoding the entries within the header.  Otherwise we just ignore the this line and get the next one
            startIndex = testLine.indexOf("  --", 0);
            if (0 == startIndex) {
                // We found an entry, scan for the terminator character
                // This can be a ' ', '=', or '['.  However we process
                // each of the terminators differently.
                startIndex += 4;  // Get past the entry marker

                // Get the entry name and update the endIndex for further searches
                EntryName = getEntryName(testLine, startIndex, &endIndex);

                // Check to see if we are supposed to ignore this entry
                if (!m_PersistantData.GetIgnoredEntryNameList().contains(EntryName)) {
                    // We Got an entry name that we can populate
                    newEntry = (EntryData*)new EntryData(EntryName);
                    if (NULL != newEntry) {
                        // POPULATE THE ENTRY DATA STRUCTURE
                        // Figure out if we got an "=" or "[", but we start at the endindex of the EntryName to
                        // get the variable and then the info text
                        // Also if we get a " " or a "\n" we dont need a variable, but we do find the infotext
                        startIndex = endIndex;
                        endIndexChar = testLine[startIndex];
                        switch (endIndexChar.toLatin1()) {

                            case '=' :  // for "=VAR" string
                                // Skip over the "=" character
                                startIndex++;

                                // Get the Variable Name
                                VarName = getVariableName(testLine, startIndex, &endIndex);

                                // Start Looking for the InfoText after the variable
                                startIndex = endIndex;
                                InfoText = getInfoTextAndFindNextLineNormal(Stream, testLine, startIndex);

                                // Now fill the Header Entry Data Structure
                                newEntry->setConfigVariableName(VarName);
                                newEntry->setConfigInfoText(InfoText);
                                newEntry->setConfigVariableRequired(true);
                                newEntry->setConfigVariableExists(true);
                                newEntry->setConfigEntryLineNumber(m_currentFilelinecount);

//                                dbgmsg = "Found ENTRY = " + EntryName +
//                                        "\nWith REQUIRED VAR = " + VarName +
//                                        "\nWith INFOTEXT = " + InfoText +
//                                        "\non Line  " + QString::number(m_currentFilelinecount);
//                                QMessageBox::critical(NULL, "DEBUG", dbgmsg);
                            break;

                            case '[' : // for "[=VAR]" string
                                // Skip over the "[=" characters
                                startIndex++;
                                startIndex++;

                                // Get the Variable Name
                                VarName = getVariableName(testLine, startIndex, &endIndex);

                                // Start Looking for the InfoText after the variable
                                startIndex = endIndex;

                                // Skip over the "]" character
                                startIndex++;
                                InfoText = getInfoTextAndFindNextLineNormal(Stream, testLine, startIndex); // Will advance nextLine

                                // Now fill the Header Entry Data Structure
                                newEntry->setConfigVariableName(VarName);
                                newEntry->setConfigInfoText(InfoText);
                                newEntry->setConfigVariableRequired(false);
                                newEntry->setConfigVariableExists(true);
                                newEntry->setConfigEntryLineNumber(m_currentFilelinecount);

//                                dbgmsg = "Found ENTRY = " + EntryName +
//                                        "\nWith OPTIONAL VAR = " + VarName +
//                                        "\nWith INFOTEXT = " + InfoText +
//                                        "\non Line  " + QString::number(m_currentFilelinecount);
//                                QMessageBox::critical(NULL, "DEBUG", dbgmsg);
                            break;

                            default :
                            case '\n' :
                            // Start Looking for the InfoText
                            InfoText = getInfoTextAndFindNextLineNormal(Stream, testLine, startIndex); // Will advance nextLine

                            // Now fill the Header Entry Data Structure
                            newEntry->setConfigVariableName(QString(""));
                            newEntry->setConfigInfoText(InfoText);
                            newEntry->setConfigVariableRequired(false);
                            newEntry->setConfigVariableExists(false);
                            newEntry->setConfigEntryLineNumber(m_currentFilelinecount);

//                            dbgmsg = "Found ENTRY = " + EntryName +
//                                    "\nWith NO VARS"
//                                    "\nWith INFOTEXT = " + InfoText +
//                                    "\non Line  " + QString::number(m_currentFilelinecount);
//                            QMessageBox::critical(NULL, "DEBUG", dbgmsg);
                            break;
                       }
                       pHeaderData->addEntryData(newEntry);
                    } else {
                        errormsg = tr("ERROR: Could not create EntryData for Entry '") + EntryName + tr("' in configure file.  Are you out of memory?");
                        m_ParsingErrorList += errormsg;
                    }
                } else {
                    // Entry was to be ignored, get the next line to look at
                    m_nextLine = Stream.readLine();
                    m_nextFilelinecount++;
                }
            } else {
                // We did not find an entry, get the next line to look at
                m_nextLine = Stream.readLine();
                m_nextFilelinecount++;
            }
        } else {
            // HANDLE SPECIAL TYPE HEADER BLOCK
            // Look for "  " which is the indicator of an entry at the start of the line.  If found, then we dig deeper to start
            // decoding the entries within the header.  Otherwise we just ignore the this line and get the next one
            startIndex = testLine.indexOf("  ", 0);
            if (0 == startIndex) {
                // We found an entry, scan for the terminator character
                // This can be a ' ', '=', or '['.  However we process
                // each of the terminators differently.
                startIndex += 2;  // Get past the entry marker

                // Get the entry name and update the endIndex for further searches
                EntryName = getEntryName(testLine, startIndex, &endIndex);

                // Check to see if we are supposed to ignore this entry
                if (!m_PersistantData.GetIgnoredEntryNameList().contains(EntryName)) {
                    // We Got an entry name that we can populate
                    newEntry = (EntryData*)new EntryData(EntryName);
                    if (NULL != newEntry) {
                        // POPULATE THE ENTRY DATA STRUCTURE
                        // Figure out if we got an "=" or "[", but we start at the endindex of the EntryName to
                        // get the variable and then the info text
                        // Also if we get a " " or a "\n" we dont need a variable, but we do find the infotext
                        startIndex = endIndex;
                        endIndexChar = testLine[startIndex];
                        switch (endIndexChar.toLatin1()) {

                            default :
                            case '\n' :
                            // Start Looking for the InfoText
                            InfoText = getInfoTextAndFindNextLineSpecial(Stream, testLine, startIndex); // Will advance nextLine

                            // Now fill the Header Entry Data Structure
                            newEntry->setConfigVariableName(QString(""));
                            newEntry->setConfigInfoText(InfoText);
                            newEntry->setConfigVariableRequired(true);
                            newEntry->setConfigVariableExists(true);
                            newEntry->setConfigEntryLineNumber(m_currentFilelinecount);

//                            dbgmsg = "Found ENTRY = " + EntryName +
//                                    "\nWith NO VARS"
//                                    "\nWith INFOTEXT = " + InfoText +
//                                    "\non Line  " + QString::number(m_currentFilelinecount);
//                            QMessageBox::critical(NULL, "DEBUG", dbgmsg);
                            break;
                       }
                       pHeaderData->addEntryData(newEntry);
                    } else {
                        errormsg = tr("ERROR: Could not create EntryData for Entry '") + EntryName + tr("' in configure file.  Are you out of memory?");
                        m_ParsingErrorList += errormsg;
                    }
                } else {
                    // Entry was to be ignored, get the next line to look at
                    m_nextLine = Stream.readLine();
                    m_nextFilelinecount++;
                }
            } else {
                // We did not find an entry, get the next line to look at
                m_nextLine = Stream.readLine();
                m_nextFilelinecount++;
            }
        }
    }  // while
}

QString ConfigFileParser::getEntryName(QString& testLine, int startIndex, int* endIndexRtn)
{
    QString rtnStr = "";
    int endIndex;

    // Scan the testLine from the startIndex for the terminator
    // character of the Entry.  This can be a ' ', or '[' or a '='.
    endIndex = testLine.indexOf(QRegExp("[\[\\s=]"), startIndex);
    if (-1 != endIndex) {
        // Found the end, copy it and return the endIndex to return
        rtnStr = testLine.mid(startIndex, endIndex - startIndex);
    } else {
        // We found the start signature of " --", but we did not find a terminator,
        // Just copy the entire line.  This happens on some entries that have info text on the
        // next line
        rtnStr = testLine.mid(startIndex, -1);
        endIndex = testLine.count();
    }

    // Return the EndIndex via the variable
    *endIndexRtn = endIndex;
    return rtnStr;  // Return the string we found
}

QString ConfigFileParser::getVariableName(QString& testLine, int startIndex, int* endIndexRtn)
{
    QString rtnStr = "";
    int endIndex;

    // Scan the testLine from the startIndex for the terminator
    // character of the varible.  This can be a ' ', or ']'.
    endIndex = testLine.indexOf(QRegExp("[\\]\\s]"), startIndex);
    if (-1 != endIndex) {
        rtnStr = testLine.mid(startIndex, endIndex - startIndex);
    } else {
        // We did not find a terminator, Just copy the remainder of the line.
        // This happens on some entries that have info text on the next line
        rtnStr = testLine.mid(startIndex, -1);
        endIndex = testLine.count();
    }

    // Return the EndIndex via the variable
    *endIndexRtn = endIndex;
    return rtnStr;  // Return the string we found
}

QString ConfigFileParser::getInfoTextAndFindNextLineNormal(QTextStream& Stream, QString& testLine, int startIndex)
{
    QString tempStr = "";
    QString rtnStr = "";
    bool    contSearch;

    // Scan the testLine from the startIndex till the end of the line
    tempStr = testLine.mid(startIndex, -1);
    // Trim off any leading or trailing whitespace and append to the return string
    tempStr = tempStr.trimmed();
    rtnStr = rtnStr + tempStr;

    // Now read in the next line if it is not empty or does not contain a " --"
    // (to mark an Entry)  or an "_ACEOF", then we append it to the return
    contSearch = true;
    do {
        m_nextLine = Stream.readLine();
        m_nextFilelinecount++;
        // Search the Line for
        if ((false == m_nextLine.isEmpty()) && (-1 == m_nextLine.indexOf(" --", 0)) && (-1 == m_nextLine.indexOf("_ACEOF", 0))) {
            // Copy the line and append it to the return string
            tempStr = m_nextLine;
            // Trim off any leading or trailing whitespace and append to the return string
            tempStr = tempStr.trimmed();
            rtnStr = rtnStr + " ";
            rtnStr = rtnStr + tempStr;
        } else {
            // Stop searching
            contSearch = false;
        }
    } while (true == contSearch);

    return rtnStr;  // Return the string we found
}

QString ConfigFileParser::getInfoTextAndFindNextLineSpecial(QTextStream& Stream, QString& testLine, int startIndex)
{
    QString tempStr = "";
    QString rtnStr = "";
    bool    contSearch;

    // Scan the testLine from the startIndex till the end of the line
    tempStr = testLine.mid(startIndex, -1);
    // Trim off any leading or trailing whitespace and append to the return string
    tempStr = tempStr.trimmed();
    rtnStr = rtnStr + tempStr;

    // Now read in the next line if it is not empty or DOEScontain a "   " at index 0
    // (to mark a NON-Entry)  or an "_ACEOF", then we append it to the return
    contSearch = true;
    do {
        m_nextLine = Stream.readLine();
        m_nextFilelinecount++;
        // Search the Line for
        if ((false == m_nextLine.isEmpty()) && (-1 == m_nextLine.indexOf("_ACEOF", 0)) && (0 == m_nextLine.indexOf("   ", 0))) {
            // Copy the line and append it to the return string
            tempStr = m_nextLine;
            // Trim off any leading or trailing whitespace and append to the return string
            tempStr = tempStr.trimmed();
            rtnStr = rtnStr + " ";
            rtnStr = rtnStr + tempStr;
        } else {
            // Stop searching
            contSearch = false;
        }
    } while (true == contSearch);

    return rtnStr;  // Return the string we found
}
