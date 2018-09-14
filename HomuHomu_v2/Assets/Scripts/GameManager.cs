using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour {
    private static GameManager _gameManager = null;
    public static GameManager CurrentGameManager {
        get { return _gameManager; }
    }
}
