#ifndef APPLICATION_SPECIFICATION_INCLUDED
#define APPLICATION_SPECIFICATION_INCLUDED

class ApplicationSpecification {
public:
	int width = 800;
	int height = 600;
	const char *title = "GAMMA";

	float mouseSens = 0.05f;
    float keySens = 10.f;
};

#endif