#ifndef APPLICATION_SPECIFICATION_INCLUDED
#define APPLICATION_SPECIFICATION_INCLUDED

class ApplicationSpecification {
public:
	int width = 800;
	int height = 600;
	const char *title = "GAMMA";

	float mouseSens = 0.01f;
    float keySens = 10.f;
    float moveSpeed = 10.f;
};

#endif