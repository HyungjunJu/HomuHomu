using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class TitleController : MonoBehaviour {
    public static TitleController Controller { private set; get; }

    private void Awake()
    {
        Controller = this;
    }

    // Use this for initialization
    void Start() {

    }

    // Update is called once per frame
    void Update() {
        if (Input.GetKeyDown(KeyCode.Alpha1)) {
            OnButtonClick(1);
        }
        if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            OnButtonClick(2);
        }
        if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            OnButtonClick(3);
        }
        if (Input.GetKeyDown(KeyCode.Alpha4))
        {
            OnButtonClick(4);
        }
    }

    public void OnButtonClick(int index) {
        switch (index) {
            case 1:
                Debug.Log("Story");
                break;
            case 2:
                Debug.Log("Tutorial");
                break;
            case 3:
                LoadTestScene();
                break;
            case 4:
                GameExit();
                break;
        }
    }

    void LoadTestScene() {
        Debug.Log("Load Test Scene");
        SceneManager.LoadScene("TestSCene");
    }

    void GameExit()
    {
        Debug.Log("Exit Game");
        Application.Quit();
    }
}
