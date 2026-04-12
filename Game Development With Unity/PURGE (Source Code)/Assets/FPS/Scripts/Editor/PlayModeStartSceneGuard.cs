using UnityEditor;
using UnityEditor.SceneManagement;

namespace Unity.FPS.EditorTools
{
    [InitializeOnLoad]
    public static class PlayModeStartSceneGuard
    {
        const string IntroMenuScenePath = "Assets/FPS/Scenes/IntroMenu.unity";

        static PlayModeStartSceneGuard()
        {
            SceneAsset introMenuScene = AssetDatabase.LoadAssetAtPath<SceneAsset>(IntroMenuScenePath);
            if (introMenuScene != null)
            {
                EditorSceneManager.playModeStartScene = introMenuScene;
            }
        }
    }
}