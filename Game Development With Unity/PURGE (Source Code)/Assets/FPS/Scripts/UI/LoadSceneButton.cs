using UnityEngine;
using UnityEngine.SceneManagement;

namespace Unity.FPS.UI
{
    public class LoadSceneButton : MonoBehaviour
    {
        public string SceneName = "";

        public void LoadTargetScene()
        {
            if (string.IsNullOrWhiteSpace(SceneName))
                return;

            SceneManager.LoadScene(SceneName);
        }
    }
}