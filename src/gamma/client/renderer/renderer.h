
class Renderer {
public:
    void Clear();
    void RenderScene();
private:
    void SetupFrame();
    void SetFrustum();
    void DrawWorld();
};