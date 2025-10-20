#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

using namespace std;   

//estructura de hospital la doctoresa 

// Estructura HistorialMedico
struct HistorialMedico {
    int idConsulta;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
};

// Estructura Paciente
struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];

    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    char alergias[500];
    char observaciones[500];

    bool activo;
};

// Estructura Doctor
struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int añosdeExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];

    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool disponible;
};

// Estructura Cita
struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;

    Doctor* doctores;
    int cantidadDoctores;
    int capacidadDoctores;

    Cita* citas;
    int cantidadCitas;
    int capacidadCitas;

    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;

    //gestion del paciente
};
Hospital* inicializarHospital(const char* nombre, int capacidadInicial) {
    Hospital* hospital = new Hospital;
    strncpy(hospital->nombre, nombre, 100);
    strcpy(hospital->direccion, "");
    strcpy(hospital->telefono, "");

    hospital->capacidadPacientes = capacidadInicial;
    hospital->cantidadPacientes = 0;
    hospital->pacientes = new Paciente[capacidadInicial];

    hospital->capacidadDoctores = capacidadInicial;
    hospital->cantidadDoctores = 0;
    hospital->doctores = new Doctor[capacidadInicial];

    hospital->capacidadCitas = capacidadInicial * 2;
    hospital->cantidadCitas = 0;
    hospital->citas = new Cita[hospital->capacidadCitas];

    hospital->siguienteIdPaciente = 1;
    hospital->siguienteIdDoctor = 1;
    hospital->siguienteIdCita = 1;
    hospital->siguienteIdConsulta = 1;

    return hospital;
}
Paciente* buscarPacientePorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            return &hospital->pacientes[i];
        }
    }
    return nullptr;
}
void redimensionarArrayPacientes(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadPacientes * 2;
    Paciente* nuevoArray = new Paciente[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        nuevoArray[i] = hospital->pacientes[i];
    }

    delete[] hospital->pacientes;
    hospital->pacientes = nuevoArray;
    hospital->capacidadPacientes = nuevaCapacidad;
}

Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedula) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcasecmp(hospital->pacientes[i].cedula, cedula) == 0) {
            return &hospital->pacientes[i];
        }
    }
    return nullptr;
}
bool validarCedula(const char* cedula) {
    if (cedula == nullptr || strlen(cedula) == 0) return false;
    if (strlen(cedula) > 20) return false;
    return true;
}

char toLower(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

bool contieneSubcadena(const char* haystack, const char* needle) {
    int lenH = strlen(haystack);
    int lenN = strlen(needle);

    if (lenN == 0) return true;

    for (int i = 0; i <= lenH - lenN; i++) {
        bool match = true;
        for (int j = 0; j < lenN; j++) {
            if (toLower(haystack[i + j]) != toLower(needle[j])) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}
Paciente** buscarPacientesPorNombre(Hospital* hospital, const char* nombre, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (contieneSubcadena(hospital->pacientes[i].nombre, nombre)) {
            (*cantidad)++;
        }
    }

    if (*cantidad == 0) return nullptr;

    Paciente** resultados = new Paciente*[*cantidad];
    int index = 0;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (contieneSubcadena(hospital->pacientes[i].nombre, nombre)) {
            resultados[index++] = &hospital->pacientes[i];
        }
    }
    return resultados;
}

Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido, const char* cedula, int edad, char sexo) {
    if (!validarCedula(cedula)) return nullptr;

    if (buscarPacientePorCedula(hospital, cedula) != nullptr) return nullptr;

    if (hospital->cantidadPacientes >= hospital->capacidadPacientes) {
        redimensionarArrayPacientes(hospital);
    }

    Paciente& nuevo = hospital->pacientes[hospital->cantidadPacientes];
    nuevo.id = hospital->siguienteIdPaciente++;
    strncpy(nuevo.nombre, nombre, 50);
    strncpy(nuevo.apellido, apellido, 50);
    strncpy(nuevo.cedula, cedula, 20);
    nuevo.edad = edad;
    nuevo.sexo = sexo;
    strcpy(nuevo.tipoSangre, "");
    strcpy(nuevo.telefono, "");
    strcpy(nuevo.direccion, "");
    strcpy(nuevo.email, "");
    strcpy(nuevo.alergias, "");
    strcpy(nuevo.observaciones, "");
    nuevo.activo = true;

    nuevo.capacidadHistorial = 5;
    nuevo.cantidadConsultas = 0;
    nuevo.historial = new HistorialMedico[nuevo.capacidadHistorial];

    nuevo.capacidadCitas = 5;
    nuevo.cantidadCitas = 0;
    nuevo.citasAgendadas = new int[nuevo.capacidadCitas];

    hospital->cantidadPacientes++;
    return &nuevo;
}
bool actualizarPaciente(Hospital* hospital, int id) {
    Paciente* p = buscarPacientePorId(hospital, id);
    if (!p) return false;

    cout << "Nuevo teléfono: ";
    cin.getline(p->telefono, 15);
    cout << "Nueva dirección: ";
    cin.getline(p->direccion, 100);
    cout << "Nuevo email: ";
    cin.getline(p->email, 50);
    cout << "Alergias: ";
    cin.getline(p->alergias, 500);
    cout << "Observaciones: ";
    cin.getline(p->observaciones, 500);

    return true;
}
bool eliminarPaciente(Hospital* hospital, int id) {
    int index = -1;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;

    delete[] hospital->pacientes[index].historial;
    delete[] hospital->pacientes[index].citasAgendadas;

    for (int i = index; i < hospital->cantidadPacientes - 1; i++) {
        hospital->pacientes[i] = hospital->pacientes[i + 1];
    }

    hospital->cantidadPacientes--;
    return true;
}
void listarPacientes(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║                    LISTA DE PACIENTES                      ║\n";
    cout << "╠═════╦═════════════════════╦══════════════╦══════╦══════════╣\n";
    cout << "║ ID  ║ NOMBRE COMPLETO     ║ CÉDULA       ║ EDAD ║ CONSULTAS║\n";
    cout << "╠═════╬═════════════════════╬══════════════╬══════╬══════════╣\n";

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        cout << "║ " << setw(4) << p.id << " ║ "
             << setw(20) << p.nombre << " " << p.apellido << " ║ "
             << setw(12) << p.cedula << " ║ "
             << setw(4) << p.edad << " ║ "
             << setw(8) << p.cantidadConsultas << " ║\n";
    }

    cout << "╚═════╩═════════════════════╩══════════════╩══════╩══════════╝\n";
}

//historial medico del pasciente 
void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta) {
    if (paciente->cantidadConsultas >= paciente->capacidadHistorial) {
        int nuevaCapacidad = paciente->capacidadHistorial * 2;
        HistorialMedico* nuevoHistorial = new HistorialMedico[nuevaCapacidad];

        for (int i = 0; i < paciente->cantidadConsultas; i++) {
            nuevoHistorial[i] = paciente->historial[i];
        }

        delete[] paciente->historial;
        paciente->historial = nuevoHistorial;
        paciente->capacidadHistorial = nuevaCapacidad;
    }

    paciente->historial[paciente->cantidadConsultas] = consulta;
    paciente->cantidadConsultas++;
}
HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad) {
    *cantidad = paciente->cantidadConsultas;
    return paciente->historial;
}
void mostrarHistorialMedico(Paciente* paciente) {
    cout << "╔════════════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                      HISTORIAL MÉDICO DEL PACIENTE                                 ║\n";
    cout << "╠════╦════════════╦═══════╦══════════════════════╦════════════╦══════════════════════╣\n";
    cout << "║ ID ║ FECHA      ║ HORA  ║ DIAGNÓSTICO          ║ DOCTOR ID  ║ COSTO                ║\n";
    cout << "╠════╬════════════╬═══════╬══════════════════════╬════════════╬══════════════════════╣\n";

    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << "║ " << setw(3) << h.idConsulta << " ║ "
             << setw(10) << h.fecha << " ║ "
             << setw(5) << h.hora << " ║ "
             << setw(22) << h.diagnostico << " ║ "
             << setw(10) << h.idDoctor << " ║ "
             << setw(20) << fixed << setprecision(2) << h.costo << " ║\n";
    }

    cout << "╚════╩════════════╩═══════╩══════════════════════╩════════════╩══════════════════════╝\n";
}
HistorialMedico* obtenerUltimaConsulta(Paciente* paciente) {
    if (paciente->cantidadConsultas == 0) return nullptr;
    return &paciente->historial[paciente->cantidadConsultas - 1];
}
//
Doctor* buscarDoctorPorCedula(Hospital* hospital, const char* cedula);
void redimensionarArrayDoctores(Hospital* hospital);

Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido, const char* cedula,
                    const char* especialidad, int aniosExperiencia, float costoConsulta) {
    if (buscarDoctorPorCedula(hospital, cedula) != nullptr) return nullptr;

    if (hospital->cantidadDoctores >= hospital->capacidadDoctores) {
        redimensionarArrayDoctores(hospital);
    }

    Doctor& nuevo = hospital->doctores[hospital->cantidadDoctores];
    nuevo.id = hospital->siguienteIdDoctor++;
    strncpy(nuevo.nombre, nombre, 50);
    strncpy(nuevo.apellido, apellido, 50);
    strncpy(nuevo.cedula, cedula, 20);
    strncpy(nuevo.especialidad, especialidad, 50);
    nuevo.añosdeExperiencia = aniosExperiencia;
    nuevo.costoConsulta = costoConsulta;
    strcpy(nuevo.horarioAtencion, "");
    strcpy(nuevo.telefono, "");
    strcpy(nuevo.email, "");
    nuevo.disponible = true;

    nuevo.capacidadPacientes = 5;
    nuevo.cantidadPacientes = 0;
    nuevo.pacientesAsignados = new int[nuevo.capacidadPacientes];

    nuevo.capacidadCitas = 10;
    nuevo.cantidadCitas = 0;
    nuevo.citasAgendadas = new int[nuevo.capacidadCitas];

    hospital->cantidadDoctores++;
    return &nuevo;
}


Doctor* buscarDoctorPorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            return &hospital->doctores[i];
        }
    }
    return nullptr;
}
Doctor* buscarDoctorPorCedula(Hospital* hospital, const char* cedula) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].cedula, cedula) == 0) {
            return &hospital->doctores[i];
        }
    }
    return nullptr;
}
    
void redimensionarArrayDoctores(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadDoctores * 2;
    Doctor* nuevoArray = new Doctor[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        nuevoArray[i] = hospital->doctores[i];
    }

    delete[] hospital->doctores;
    hospital->doctores = nuevoArray;
    hospital->capacidadDoctores = nuevaCapacidad;
}
Doctor** buscarDoctoresPorEspecialidad(Hospital* hospital, const char* especialidad, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].especialidad, especialidad) == 0) {
            (*cantidad)++;
        }
    }

    if (*cantidad == 0) return nullptr;

    Doctor** resultados = new Doctor*[*cantidad];
    int index = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].especialidad, especialidad) == 0) {
            resultados[index++] = &hospital->doctores[i];
        }
    }
    return resultados;
}
bool asignarPacienteADoctor(Doctor* doctor, int idPaciente) {
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) return false;
    }

    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCapacidad = doctor->capacidadPacientes * 2;
        int* nuevoArray = new int[nuevaCapacidad];
        for (int i = 0; i < doctor->cantidadPacientes; i++) {
            nuevoArray[i] = doctor->pacientesAsignados[i];
        }
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevoArray;
        doctor->capacidadPacientes = nuevaCapacidad;
    }

    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    return true;
}

bool removerPacienteDeDoctor(Doctor* doctor, int idPaciente) {
    int index = -1;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;

    for (int i = index; i < doctor->cantidadPacientes - 1; i++) {
        doctor->pacientesAsignados[i] = doctor->pacientesAsignados[i + 1];
    }
    doctor->cantidadPacientes--;
    return true;
}
void listarPacientesDeDoctor(Hospital* hospital, int idDoctor) {
    Doctor* doctor = buscarDoctorPorId(hospital, idDoctor);
    if (!doctor) return;

    cout << "Pacientes asignados al Dr. " << doctor->nombre << " " << doctor->apellido << ":\n";
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        Paciente* p = buscarPacientePorId(hospital, doctor->pacientesAsignados[i]);
        if (p) {
            cout << "- " << p->nombre << " " << p->apellido << " (ID: " << p->id << ")\n";
        }
    }
}

void listarDoctores(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║                    LISTA DE DOCTORES                       ║\n";
    cout << "╠═════╦═════════════════════╦══════════════╦═════════════════╣\n";
    cout << "║ ID  ║ NOMBRE COMPLETO     ║ CÉDULA       ║ ESPECIALIDAD    ║\n";
    cout << "╠═════╬═════════════════════╬══════════════╬═════════════════╣\n";

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        cout << "║ " << setw(4) << d.id << " ║ "
             << setw(20) << d.nombre << " " << d.apellido << " ║ "
             << setw(12) << d.cedula << " ║ "
             << setw(15) << d.especialidad << " ║\n";
    }

    cout << "╚═════╩═════════════════════╩══════════════╩═════════════════╝\n";
}

bool eliminarDoctor(Hospital* hospital, int id) {
    int index = -1;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;

    delete[] hospital->doctores[index].pacientesAsignados;
    delete[] hospital->doctores[index].citasAgendadas;

    for (int i = index; i < hospital->cantidadDoctores - 1; i++) {
        hospital->doctores[i] = hospital->doctores[i + 1];
    }

    hospital->cantidadDoctores--;
    return true;
}


