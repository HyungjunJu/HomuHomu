using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class ResourceLoader {
    private static ResourceLoader _loader = null;
    public static ResourceLoader Loader {
        get {
            if (_loader == null) {
                _loader = new ResourceLoader();
            }
            return _loader;
        }
    }

    public ResourceLoader() {
        //load default info
    }

}