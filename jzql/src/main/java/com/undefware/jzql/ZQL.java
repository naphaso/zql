package com.undefware.jzql;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by wolong on 12/30/13.
 */
public class ZQL {
    static {
        System.loadLibrary("zql");
    }

    public ZQL(String endpoint) {
        connect(endpoint);
    }

    

    public void getById(String database, String table, int id) {
        int requestId = select(database, table, id);

    }

    private void processResponse(Object[][] results) {
        System.out.println("processing response");
    }

    private native void connect(String endpoint);
    private native int select(String database, String table, int id);
}
