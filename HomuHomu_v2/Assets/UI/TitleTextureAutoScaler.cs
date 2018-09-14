using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
[ExecuteInEditMode]
#endif
public class TitleTextureAutoScaler : MonoBehaviour {
    public SpriteRenderer titleTextureRenderer;
    private Sprite _currentSprite = null;

    private void Update()
    {
        if (titleTextureRenderer == null) return;
        if (titleTextureRenderer.sprite == null) return;
        if (_currentSprite == titleTextureRenderer.sprite) return;

        float x, y, pixel;
        _currentSprite = titleTextureRenderer.sprite;
        x = _currentSprite.texture.width;
        y = _currentSprite.texture.height;
        pixel = _currentSprite.pixelsPerUnit;
    }
}