
class Vector2 {
public:
	float x;
	float y;

	Vector2() {
		this->x = 0;
		this->y = 0;
	}

	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	static Vector2 Zero() {
		return Vector2(0.0f, 0.0f);
	}

	bool operator!=(const Vector2 &src) const {
		return (src.x != x) || (src.y != y);
	}

	Vector2 &operator+=(const Vector2 &v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2 &operator-=(const Vector2 &v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
};

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	static Vector3 Zero() {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
};

struct Matrix4x4 {
    float m[4][4];//kmods
};

/*
struct Matrix4x4 {// work 
	float M11, M12, M13, M14;
    float M21, M22, M23, M24;
    float M31, M32, M33, M34;
    float M41, M42, M43, M44;
};
  */

//obtenha a posição certar , da tela
 int screenWidth = 1920;
 int screenHeight = 1080;


//criar um ponteiro , no centro
const Vector2 crosshair = {screenWidth / 2.0f, screenHeight / 2.0f};


/*
I thought about creating one worldToScreen  just to draw the pointer in the projector space, from the visualization 

example 

but use yours if you have one, or create your own 

Vector2 worldToScreen(Vector3 worldPos, Matrix4x4 viewMatrix) {
    Vector2 screenPos;
    float w = viewMatrix.m[3][0] * worldPos.x + viewMatrix.m[3][1] * worldPos.y +
    viewMatrix.m[3][2] * worldPos.z + viewMatrix.m[3][3];
    if (w < 0.1f) { // Fora da tela
        screenPos.x = -1;
        screenPos.y = -1;
        return screenPos;
    }
    float x = viewMatrix.m[0][0] * worldPos.x + viewMatrix.m[0][1] * worldPos.y +
              viewMatrix.m[0][2] * worldPos.z + viewMatrix.m[0][3];
    float y = viewMatrix.m[1][0] * worldPos.x + viewMatrix.m[1][1] * worldPos.y +
              viewMatrix.m[1][2] * worldPos.z + viewMatrix.m[1][3];
    x /= w;
    y /= w;
    screenPos.x = (screenWidth / 2.0f) * (1.0f + x);
    screenPos.y = (screenHeight / 2.0f) * (1.0f - y);
    return screenPos;
}
*/


bool injectTouch(Vector2 targetPos) {//Dispositivo de toque
    int fd = open("/dev/input/event0", O_RDWR);
    if (fd < 0) {
        std::cout << "Failed to open input device\n";
        return false;
    }

    struct input_event event;
    // Iniciar toque ----(touch down)
    event.type = EV_KEY;
    event.code = BTN_TOUCH;
    event.value = 1; // Pressionar
    write(fd, &event, sizeof(event));

    // Mover para a posição x
    event.type = EV_ABS;
    event.code = ABS_X;
    event.value = static_cast<int>(targetPos.x);
    write(fd, &event, sizeof(event));

    // Mover para a posição y
    event.code = ABS_Y;
    event.value = static_cast<int>(targetPos.y);
    write(fd, &event, sizeof(event));

    // Sincronizar
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(fd, &event, sizeof(event));

    // Finalizar toque (touch up)
    event.type = EV_KEY;
    event.code = BTN_TOUCH;
    event.value = 0; // Soltar
    write(fd, &event, sizeof(event));

    // Sincronizar novamente
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(fd, &event, sizeof(event));

    close(fd);
    return true;
}



//I wanted to create a way to get the distance to pull the player's head offset 

float calculateDistance(Vector3 a, Vector3 b) {
    return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) + std::pow(b.z - a.z, 2));
}

Vector3 getHeadPosition(uintptr_t entityBase) {
    Vector3 headPos;
    uintptr_t headAddress = entityBase + 0xC2;//getHead
    headPos.x = Read<float>(headAddress);
    headPos.y = Read<float>(headAddress + 0x4); // y
    headPos.z = Read<float>(headAddress + 0x8); // z
    return headPos;
}

//Criar movimento
Vector2 smoothTouch(Vector2 currentPos, Vector2 targetPos, float smoothFactor) {
    Vector2 smoothedPos;
    smoothedPos.x = currentPos.x + (targetPos.x - currentPos.x) * smoothFactor;
    smoothedPos.y = currentPos.y + (targetPos.y - currentPos.y) * smoothFactor;
    return smoothedPos;
}




void aimbot() {
    // Endereço da lista de entidades (substitua pelo offset real)
    uintptr_t entityListBase = getRealOffset(0x64);
    int entityCount = Read<int>(entityListBase + 0x8);

    // Endereço da ViewMatrix
    uintptr_t viewMatrixAddr = getRealOffset(0x789abc);
    Matrix4x4 viewMatrix = Read<Matrix4x4>(viewMatrixAddr);

    // Posição do jogador
    uintptr_t localPlayer = Read<uintptr_t>(getRealOffset(0x36C));
    Vector3 localPos = Read<Vector3>(localPlayer + 0x200);

    if (checkTouchInput()) { // Detectar // touch
        Vector3 closestHead = {0, 0, 0};
        float minDistance = FLT_MAX;
        uintptr_t closestEntity = 0;
        for (int i = 0; i < entityCount; i++) {
            uintptr_t entity = Read<uintptr_t>(entityListBase + (i * sizeof(uintptr_t)));
            if (entity && entity != localPlayer) { // Ignorar jogador
                float health = Read<float>(entity + 0xED4); //Enemy player's life 
                if (health > 0) {
                    Vector3 headPos = getHeadPosition(entity);
                    float distance = calculateDistance(localPos, headPos);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestHead = headPos;
                        closestEntity = entity;
                    }
                }
            }
        }
        if (closestEntity) {
            Vector2 headScreenPos = worldToScreen(closestHead, viewMatrix);
            if (headScreenPos.x >= 0 && headScreenPos.y >= 0) {
                Vector2 smoothedPos = smoothTouch(crosshair, headScreenPos, 0.1f);
                if (injectTouch(smoothedPos)) {
                    } else {
                }
            }
        }
    }
}

int main() {
target_pid = find_pid("com.edkongames.mobs");
    if (target_pid == -1) {
        return 1;
}
 il2cppBase = get_module_base("libil2cpp.so");
   if (il2cppBase > 0) {
    return;
 }
while (true) {
        aimbot();
        usleep(10000);
    }
    return 0;
}
