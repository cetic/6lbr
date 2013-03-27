/*
 *
 * 2012-03-01
 * Andre Frimberger:
 *   * Implemented pseudo terminal support for Java
 *
 *
 * Copyright (c) 2012, Andre Frimberger
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


package de.fau.cooja.plugins;

import org.apache.log4j.Logger;

import java.io.*;

/**
 * User: andre
 * Date: 23.02.12
 * Time: 10:59
 */
public class LinuxPseudoTerminal {
    private static Logger logger = Logger.getLogger(LinuxPseudoTerminal.class);

    private FileDescriptor _fd_master;

    private String _slavePtsName;

    private BufferedOutputStream _ptOutputStream;
    private BufferedInputStream _ptInputStream;

    private native void setupMasterPt(FileDescriptor fdmaster) throws IOException;
    private native String getSlavePtsName(FileDescriptor fdmaster);
    private native void setupSlavePt(FileDescriptor fdmaster) throws IOException;
    private native void closePt(FileDescriptor fdmaster);

    
    private LinuxPseudoTerminal() {}

    LinuxPseudoTerminal(String pathToLibPty) {
        System.load(pathToLibPty +  "/libpty.so");
    }

    public void open() throws IOException {
        _fd_master = new FileDescriptor();

        //logger.debug("running setupMasterPt");
        setupMasterPt(_fd_master);
        //logger.debug("setupMasterPt done.");

        //logger.debug("running setupSlavePt");
        setupSlavePt(_fd_master);
        //logger.debug("setupSlavePt done.");


        //logger.debug("running getSlavePtsName");
        _slavePtsName = getSlavePtsName(_fd_master);
        //logger.debug("getSlavePtsName done: " + _slavePtsName);

        //logger.debug("opening OutputStream ");
        _ptOutputStream = new BufferedOutputStream(new FileOutputStream(_fd_master));

        //logger.debug("opening InputStream ");
        _ptInputStream = new BufferedInputStream(new FileInputStream(_fd_master));
        _ptOutputStream.flush();
    }


    public BufferedInputStream getPtInputStream() {
        return _ptInputStream;
    }

    public BufferedOutputStream getPtOutputStream() {
        return _ptOutputStream;
    }

    public String getDeviceName() {
        return _slavePtsName;
    }

    public void close() {
        try {
            _ptInputStream.close();
            _ptOutputStream.close();
            closePt(_fd_master);
        } catch (IOException e) {
            logger.fatal("Error while closing Input/Outputstream" + e);
            e.printStackTrace();
        }
    }

}
