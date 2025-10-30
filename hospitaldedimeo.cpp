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
bool validarCedula(const char* cedula);

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
    int idPaciente;
cout << "ID del paciente: ";
cin >> idPaciente;

    if (index == -1) {
        cout << " Paciente con ID " << idPaciente << " no encontrado.\n";
    }

    delete[] hospital->pacientes[index].historial;
    delete[] hospital->pacientes[index].citasAgendadas;

    for (int i = index; i < hospital->cantidadPacientes - 1; i++) {
        hospital->pacientes[i] = hospital->pacientes[i + 1];
    }

    hospital->cantidadPacientes--;
    return true;
}

void listarPacientes(Hospital* hospital) {
    cout << "\n+---------------------------------------------------------------+\n";
    cout << "|                    LISTA DE PACIENTES                        |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "| ID  | NOMBRE COMPLETO      | CEDULA        | EDAD | CONSULTAS |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";


   for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        cout << "| " << setw(3) << p.id << " | "
             << setw(20) << left << p.nombre << " | "
             << setw(13) << p.cedula << " | "
             << setw(4) << p.edad << " | "
             << setw(9) << p.capacidadHistorial << " |\n";
    }
    
    cout << "+-----+----------------------+---------------+------+-----------+\n";
}
bool contieneTexto(const char* texto, const char* patron) {
    return strstr(texto, patron) != nullptr;
}

void buscarPacientesPorNombreParcial(Hospital* hospital, const char* nombreParcial) {
    bool encontrado = false;

    cout << "+---------------------------------------------------------------+\n";
    cout << "|              PACIENTES QUE COINCIDEN CON EL NOMBRE            |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "| ID  | NOMBRE COMPLETO      | CÉDULA        | EDAD | CONSULTAS |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        if (contieneTexto(p.nombre, nombreParcial)) {
            cout << "| " << setw(3) << p.id << " | "
                 << setw(20) << left << p.nombre << " "
                 << setw(15) << p.apellido << " | "
                 << setw(13) << p.cedula << " | "
                 << setw(4) << p.edad << " | "
                 << setw(9) << p.capacidadHistorial << " |\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        cout << "|        No se encontraron pacientes con ese nombre parcial. |\n";
    }

    cout << "+-----+----------------------+---------------+------+-----------+\n";
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
void mostrarHistorialMedico(Hospital* hospital, int idPaciente) {
    Paciente* paciente = buscarPacientePorId(hospital, idPaciente);
    if (!paciente) {
        cout << " Paciente no encontrado.\n";
        return;
    }

    cout << "+===============================================================================+\n";
    cout << "|                      HISTORIAL MEDICO DEL PACIENTE                           |\n";
    cout << "+====+============+=======+======================+============+================+\n";
    cout << "| ID | FECHA      | HORA  | DIAGNOSTICO          | DOCTOR ID  | COSTO          |\n";
    cout << "+====+============+=======+======================+============+================+\n";

    for (int i = 0; i < paciente->capacidadHistorial; i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << "| " << setw(2) << h.idConsulta << " | "
             << setw(10) << h.fecha << " | "
             << setw(5) << h.hora << " | "
             << setw(22) << h.diagnostico << " | "
             << setw(10) << h.idDoctor << " | "
             << setw(14) << fixed << setprecision(2) << h.costo << " |\n";
    }

    cout << "+===============================================================================+\n";
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
    cout << "\n+===============================================================+\n";
    cout << "|                       LISTA DE DOCTORES                      |\n";
    cout << "+-----+------------------------+----------------+---------------------+\n";
    cout << "| ID  | NOMBRE COMPLETO        | CEDULA         | ESPECIALIDAD        |\n";
    cout << "+-----+------------------------+----------------+---------------------+\n";

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        // Imprimir cada doctor con formato alineado
        cout << "| " << setw(4) << d.id << " | "
             << setw(22) << d.nombre << " | "
             << setw(14) << d.cedula << " | "
             << setw(19) << d.especialidad << " |\n";
    }

    cout << "+-----+------------------------+----------------+---------------------+\n";
}


bool eliminarDoctor(Hospital* hospital, int id) {
    int index = -1;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        cout << "Doctor con ID " << id << " no encontrado.\n";
    }
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == id && strcmp(hospital->citas[i].estado, "pendiente") == 0) {
            cout << "No se puede eliminar: el doctor tiene citas pendientes.\n";
        }
        hospital->cantidadDoctores--;
    cout << "Doctor eliminado correctamente.\n";
    }
    delete[] hospital->doctores[index].pacientesAsignados;
    delete[] hospital->doctores[index].citasAgendadas;

    for (int i = index; i < hospital->cantidadDoctores - 1; i++) {
        hospital->doctores[i] = hospital->doctores[i + 1];
    }

    hospital->cantidadDoctores--;
    return true;
}

//Gestion de citas
void agendarCita(Hospital* hospital, int idPaciente, int idDoctor, const char* fecha, const char* hora) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& cita = hospital->citas[i];
        if (cita.idDoctor == idDoctor &&
            strcmp(cita.fecha, fecha) == 0 &&
            strcmp(cita.hora, hora) == 0) {
            cout << " Ya existe una cita con el doctor en esa fecha y hora.\n";
            return;
        }
    }

    // Verificar capacidad
    if (hospital->cantidadCitas >= hospital->capacidadCitas) {
        cout << " No hay espacio para más citas.\n";
        return;
    }

    // Crear nueva cita
    Cita& nueva = hospital->citas[hospital->cantidadCitas];
    nueva.id = hospital->cantidadCitas + 1;
    nueva.idPaciente = idPaciente;
    nueva.idDoctor = idDoctor;
    strcpy(nueva.fecha, fecha);
    strcpy(nueva.hora, hora);
    strcpy(nueva.estado, "pendiente");

    cout << "Cita agendada con ID: " << nueva.id << "\n";
    hospital->cantidadCitas++;

}

bool cancelarCita(Hospital* hospital, int idCita) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita) {
            *hospital->citas[i].estado = 2; // cancelada
            return true;
        }
    }
    return false;
    
}

bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico, float costo) {
    Cita* cita = nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita) {
            cita = &hospital->citas[i];
            break;
        }
    }
    if (!cita) {
        cout << " La cita con ID " << idCita << " no existe.\n";
    }
Paciente* paciente = buscarPacientePorId(hospital, cita->idPaciente);

if (!cita || cita->estado!= 0) return false;
    
HistorialMedico& nuevo = paciente->historial[paciente->capacidadHistorial];
nuevo.idConsulta = paciente->capacidadHistorial + 1;
strcpy(nuevo.fecha, cita->fecha);
strcpy(nuevo.hora, cita->hora);
strcpy(nuevo.diagnostico, diagnostico);
nuevo.idDoctor = cita->idDoctor;
nuevo.costo = costo;
paciente->capacidadHistorial++;

    *cita->estado = 1; // atendida

    HistorialMedico consulta;
    consulta.idConsulta = cita->id;
    strncpy(consulta.fecha, cita->fecha, 11);
    strncpy(consulta.hora, cita->hora, 6);
    strncpy(consulta.diagnostico, diagnostico, 200);
    consulta.idDoctor = cita->idDoctor;
    consulta.costo = costo;


    if (!paciente) return false;

    agregarConsultaAlHistorial(paciente, consulta);
    return true;
    
}


void listarCitasPorPaciente(Hospital* hospital, int idPaciente) {
    cout << "Citas del paciente ID " << idPaciente << ":\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) {
            cout << "- Cita ID " << hospital->citas[i].id << " con doctor " << hospital->citas[i].idDoctor
                 << " el " << hospital->citas[i].fecha << " a las " << hospital->citas[i].hora
                 << " (estado: " << hospital->citas[i].estado << ")\n";
        }
    }
}
void listarCitasPorDoctor(Hospital* hospital, int idDoctor) {
    cout << "Citas del doctor ID " << idDoctor << ":\n";
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) {
            cout << "- Cita ID " << hospital->citas[i].id << " con paciente " << hospital->citas[i].idPaciente
                 << " el " << hospital->citas[i].fecha << " a las " << hospital->citas[i].hora
                 << " (estado: " << hospital->citas[i].estado << ")\n";
        }
    }
}

//Validaciones y Utilidades

bool validarCedula(const char* cedula) {
    if (cedula == nullptr || strlen(cedula) == 0) return false;
    if (strlen(cedula) > 20) return false;
    return true;
}

bool validarEmail(const char* email) {
    if (email == nullptr || strlen(email) == 0) return false;
    if (strlen(email) > 100) return false;
    return strchr(email, '@') != nullptr;
}

bool validarHora(const char* hora) {
    if (strlen(hora) != 5) return false;
    return (hora[2] == ':');
}

void redimensionarArrayCitas(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadCitas * 2;
    Cita* nuevoArray = new Cita[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        nuevoArray[i] = hospital->citas[i];
    }

    delete[] hospital->citas;
    hospital->citas = nuevoArray;
    hospital->capacidadCitas = nuevaCapacidad;
}

void redimensionarCitasPaciente(Paciente* paciente) {
    int nuevaCapacidad = paciente->capacidadCitas * 2;
    int* nuevoArray = new int[nuevaCapacidad];

    for (int i = 0; i < paciente->cantidadCitas; i++) {
        nuevoArray[i] = paciente->citasAgendadas[i];
    }

    delete[] paciente->citasAgendadas;
    paciente->citasAgendadas = nuevoArray;
    paciente->capacidadCitas = nuevaCapacidad;
}

void redimensionarCitasDoctor(Doctor* doctor) {
    int nuevaCapacidad = doctor->capacidadCitas * 2;
    int* nuevoArray = new int[nuevaCapacidad];

    for (int i = 0; i < doctor->cantidadCitas; i++) {
        nuevoArray[i] = doctor->citasAgendadas[i];
    }

    delete[] doctor->citasAgendadas;
    doctor->citasAgendadas = nuevoArray;
    doctor->capacidadCitas = nuevaCapacidad;
}

void destruirHospital(Hospital* hospital) {
    // Liberar memoria de cada paciente
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        delete[] hospital->pacientes[i].citasAgendadas;   // Arreglo de citas agendadas
        delete[] hospital->pacientes[i].historial;        // Arreglo de historial médico
    }

    // Liberar arreglo de pacientes
    delete[] hospital->pacientes;

    // Liberar arreglo de doctores
    delete[] hospital->doctores;

    // Liberar arreglo de citas
    delete[] hospital->citas;

    // Opcional: poner punteros en nullptr por seguridad
    hospital->pacientes = nullptr;
    hospital->doctores = nullptr;
    hospital->citas = nullptr;

    // Reiniciar contadores
    hospital->cantidadPacientes = 0;
    hospital->cantidadDoctores = 0;
    hospital->cantidadCitas = 0;
}

int main() {
    Hospital hospital;
    hospital.capacidadPacientes = 10;
    hospital.capacidadDoctores = 10;
    hospital.capacidadCitas = 10;
    hospital.pacientes = new Paciente[hospital.capacidadPacientes];
    hospital.doctores = new Doctor[hospital.capacidadDoctores];
    hospital.citas = new Cita[hospital.capacidadCitas];
    hospital.cantidadPacientes = 0;
    hospital.cantidadDoctores = 0;
    hospital.cantidadCitas = 0;
    hospital.siguienteIdPaciente = 1;
    hospital.siguienteIdDoctor = 1;
    hospital.siguienteIdCita = 1;

    int opcion;
 do {
    
    // Mostrar el menú
    cout << "\n+--------------------------------------+\n";
    cout << "|     MENU PRINCIPAL DEL HOSPITAL      |\n";
    cout << "+--------------------------------------+\n";
    cout << "| 1. Registrar paciente                |\n";
    cout << "| 2. Registrar doctor                  |\n";
    cout << "| 3. Agendar cita                      |\n";
    cout << "| 4. Atender cita                      |\n";
    cout << "| 5. Mostrar historial de pacientes    |\n";
    cout << "| 6. Listar doctores                   |\n";
    cout << "| 7. Listar pacientes                  |\n";
    cout << "| 8. buscar paciente                   |\n";
    cout << "| 9. buscar doctor                     |\n";
    cout << "|10. eliminar paciente                 |\n";
    cout << "|11. eliminar doctor                   |\n";
    cout << "| 0. Salir                             |\n";
    cout << "+--------------------------------------+\n";
    cout << "Seleccione una opcion: ";
    cin >> opcion;

    // Validar entrada
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Entrada inválida. Intente de nuevo.\n";
        continue;
    }

    cin.ignore(); // Limpiar el buffer antes de usar getline

    switch (opcion) {
        case 1: {
            char nombre[50], apellido[50], cedula[20], tipoSangre[5];
            int edad;
            int idPaciente;
            char sexo;
            cout << "Nombre: "; cin.getline(nombre, 50);
            cout << "Apellido: "; cin.getline(apellido, 50);
            cout << "Cedula: "; cin.getline(cedula, 20);
            cout << "Edad: "; cin >> edad;
            cout << "Tipo de sangre"; cin.getline(tipoSangre, 5);
            cout << "id del paciente"; cin >> idPaciente;
            cout << "Sexo (M/F): "; cin >> sexo;
            cin.ignore();
            crearPaciente(&hospital, nombre, apellido, cedula, edad, sexo);
            break;
        }
        case 2: {
            char nombre[50], apellido[50], cedula[20], especialidad[50];
            int experiencia;
            int idDoctor;
            float costo;
            cout << "Nombre: "; cin.getline(nombre, 50);
            cout << "Apellido: "; cin.getline(apellido, 50);
            cout << "Cedula profesional: "; cin.getline(cedula, 20);
            cout << "Especialidad: "; cin.getline(especialidad, 50);
            cout << "tiempo de experiencia: "; cin >> experiencia;
            cout << "id del doctor: ";cin >> idDoctor;
            cout << "Costo de consulta: "; cin >> costo;
            cin.ignore();
            crearDoctor(&hospital, nombre, apellido, cedula, especialidad, experiencia, costo);
            break;
        }
        case 3: {
            int idPaciente, idDoctor;
            char fecha[11], hora[6];
            cout << "ID del paciente: "; cin >> idPaciente;
            cout << "ID del doctor: "; cin >> idDoctor;
            cin.ignore();
            cout << "Fecha (dd/mm/yyyy): "; cin.getline(fecha, 11);
            cout << "Hora (hh:mm): "; cin.getline(hora, 6);
            cout << "Cita ID: " << hospital.siguienteIdCita - 1 << "\n";
            agendarCita(&hospital, idPaciente, idDoctor, fecha, hora);
            break;
        }
        case 4: {
            int idCita;
            cout << "ID de la cita: "; cin >> idCita;
            cin.ignore();
            Cita* cita = nullptr;
            for (int i = 0; i < hospital.cantidadCitas; i++) {
                if (hospital.citas[i].id == idCita) {
                    cita = &hospital.citas[i];
                    break;
                }
            }
            if (!cita) {
                cout << "La cita con ID " << idCita << " no existe.\n";
                break;
            }
            char diagnostico[200];
            float costo;
            cout << "Diagnostico: "; cin.getline(diagnostico, 200);
            cout << "Costo: "; cin >> costo;
            cin.ignore();
            if (strcmp(cita->estado, "pendiente") != 0) {
                cout << "La cita no está pendiente.\n";
            } else {
                atenderCita(&hospital, idCita, diagnostico, costo);
                cout << "Cita atendida correctamente.\n";
            }
            break;
        }
        case 5: {
            int idPaciente;
            cout << "ID del paciente: "; cin >> idPaciente;
            mostrarHistorialMedico(&hospital, idPaciente); 
            break;

        }
        case 6:
            listarDoctores(&hospital);
            break;
        case 7:
            listarPacientes(&hospital);
            break;

        case 8: {
            char cedula[20];
    cout << "Ingrese la cedula del paciente: ";
    cin >> cedula;

    Paciente* paciente = buscarPacientePorCedula(&hospital, cedula);
    if (paciente) {
        cout << "Paciente encontrado:\n";
        cout << "Nombre: " << paciente->nombre << "\n";
        cout << "Apellido: " << paciente->apellido << "\n";
        cout << "Edad: " << paciente->edad << "\n";
    } else {
        cout << " Paciente no encontrado.\n";
    }
    break;

        }
        case 9: {
             char especialidad[50];
             cout << "Ingrese la especialidad a buscar: ";
             cin.ignore();
             cin.getline(especialidad, 50);
             int *cantidad;
             buscarDoctoresPorEspecialidad(&hospital, especialidad, cantidad );
             break;
        }
        case 10: {
            int idPaciente;
            cout << "ID del paciente: ";
            cin >> idPaciente;
            mostrarHistorialMedico(&hospital, idPaciente);
            break;
        }
        case 11: {
            int idDoctor;
            cout << "ID del doctor a eliminar: ";
            cin >> idDoctor;
            eliminarDoctor(&hospital, idDoctor);
            break;
        }
        case 12: {
            char nombreParcial[50];
             cout << "Ingrese parte del nombre del paciente: ";
             cin.ignore();
             cin.getline(nombreParcial, 50);
             buscarPacientesPorNombreParcial(&hospital, nombreParcial);
             break;
            }

        case 0:
            cout << "Gracias por usar el sistema del hospital.\n";
            break;
        default:
            cout << "Opción no válida. Intente de nuevo.\n";
            break;
    }

} while (opcion != 0);

// Liberar memoria
delete[] hospital.pacientes;
delete[] hospital.doctores;
delete[] hospital.citas;

destruirHospital(&hospital);

return 0;
}





