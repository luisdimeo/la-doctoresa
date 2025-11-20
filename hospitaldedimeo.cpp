#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

// ---------------- Utilidades básicas ----------------
struct ArchivoHeader {
    int cantidadRegistros; // Total escritos
    int proximoID;         // Siguiente ID
    int registrosActivos;  // Activos (no eliminados)
    int version;           // Formato
};

bool inicializarArchivo(const char* nombreArchivo) {
    fstream archivo(nombreArchivo, ios::binary | ios::out);
    if (!archivo.is_open()) return false;
    ArchivoHeader header = {0, 1, 0, 1};
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}
ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header{0, 1, 0, 1};
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return header;
}
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    archivo.seekp(0, ios::beg);
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}
long calcularPosicion(int indice, size_t tamRegistro) {
    return static_cast<long>(sizeof(ArchivoHeader)) + static_cast<long>(indice) * static_cast<long>(tamRegistro);
}

int strcasecmp_compat(const char* a, const char* b) {
    if (!a || !b) return (a == b) ? 0 : (a ? 1 : -1);
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? (*a + 32) : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? (*b + 32) : *b;
        if (ca != cb) return (unsigned char)ca - (unsigned char)cb;
        ++a; ++b;
    }
    return (unsigned char)(*a) - (unsigned char)(*b);
}
char toLower(char c) { return (c >= 'A' && c <= 'Z') ? (c + 32) : c; }
bool contieneSubcadena(const char* haystack, const char* needle) {
    int lenH = (int)strlen(haystack);
    int lenN = (int)strlen(needle);
    if (lenN == 0) return true;
    for (int i = 0; i <= lenH - lenN; i++) {
        bool match = true;
        for (int j = 0; j < lenN; j++) {
            if (toLower(haystack[i + j]) != toLower(needle[j])) { match = false; break; }
        }
        if (match) return true;
    }
    return false;
}

// ---------------- Metadatos del sistema ----------------
struct HospitalMetadata {
    int nextPacienteID = 1;
    int nextDoctorID = 1;
    int nextCitaID = 1;
    int nextHistorialID = 1;
    int cantidadPacientesActivos = 0;
    int cantidadDoctoresActivos = 0;
    int cantidadCitasActivas = 0;
};
HospitalMetadata hospital;

void guardarMetadatos() {
    ofstream file("hospital.bin", ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&hospital), sizeof(HospitalMetadata));
        file.close();
    } else {
        cerr << "ERROR: No se pudieron guardar los metadatos." << endl;
    }
}
bool inicializarSistema() {
    ifstream file("hospital.bin", ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&hospital), sizeof(HospitalMetadata));
        file.close();
        cout << "\n✅ Sistema inicializado. Metadatos cargados de hospital.bin." << endl;
        return true;
    }
    cout << "\n⚠️ hospital.bin no encontrado. Iniciando sistema con IDs en 1." << endl;
    return true;
}
void cerrarSistema() {
    guardarMetadatos();
    cout << "\nMetadatos guardados con éxito. El sistema está listo para cerrarse." << endl;
}

// ---------------- Estructuras principales ----------------
struct HistorialMedico {
    int idConsulta = 0;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor = 0;
    int pacienteID = 0;
    float costo = 0.0f;
    int siguienteConsultaID = 0;
    bool eliminado = false;
};

struct Paciente {
    int id = 0;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad = 0;
    char sexo = ' ';
    char tipoSangre[5];
    char telefono[20];
    char direccion[100];
    char email[50];

    int citasIDs[20];
    int cantidadCitas = 0;

    int primerConsultaID = 0;
    int cantidadConsultas = 0;

    char alergias[500];
    char observaciones[500];

    bool activo = true;
};

struct Doctor {
    int id = 0;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    char horarioAtencion[50];
    char telefono[20];
    char email[50];
    int experiencia = 0;
    float costo = 0.0f;
    bool activo = true;
};

struct Cita {
    int id = 0;
    int pacienteID = 0;
    int doctorID = 0;
    char fecha[11];
    char hora[6];
    char motivo[100];
    bool activa = true;
    bool completada = false;
};

// ---------------- Validaciones ----------------
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
    if (hora[2] != ':') return false;
    int hh = (hora[0]-'0')*10 + (hora[1]-'0');
    int mm = (hora[3]-'0')*10 + (hora[4]-'0');
    return (hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59);
}

// ---------------- Pacientes.bin ----------------
Paciente buscarPacientePorCedula_BIN(const char* cedula) {
    Paciente p{};
    ifstream file("pacientes.bin", ios::binary);
    if (!file.is_open()) return p;
    ArchivoHeader header{};
    file.read((char*)&header, sizeof(ArchivoHeader));
    while (file.read(reinterpret_cast<char*>(&p), sizeof(Paciente))) {
        if (p.activo && strcasecmp_compat(p.cedula, cedula) == 0) return p;
    }
    return Paciente{};
}
Paciente buscarPacientePorID_BIN(int id) {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) return Paciente{};
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Paciente p{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (p.id == id) return p;
    }
    return Paciente{};
}
bool agregarPaciente(Paciente nuevoPaciente) {
    const char* archivoBin = "pacientes.bin";
    ifstream chk(archivoBin, ios::binary);
    if (!chk.is_open()) inicializarArchivo(archivoBin);

    ArchivoHeader header = leerHeader(archivoBin);
    nuevoPaciente.id = header.proximoID;
    nuevoPaciente.activo = true;
    nuevoPaciente.cantidadCitas = 0;
    nuevoPaciente.primerConsultaID = 0;
    nuevoPaciente.cantidadConsultas = 0;
    nuevoPaciente.alergias[0] = '\0';
    nuevoPaciente.observaciones[0] = '\0';

    fstream archivo(archivoBin, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    archivo.write((char*)&nuevoPaciente, sizeof(Paciente));
    archivo.close();

    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(archivoBin, header);
    return true;
}
bool actualizarPaciente(const Paciente& pacienteModificado) {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) return false;
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();

    fstream out("pacientes.bin", ios::binary | ios::in | ios::out);
    if (!out.is_open()) return false;
    Paciente p{};
    out.seekg(sizeof(ArchivoHeader));
    for (int i = 0; i < header.cantidadRegistros; i++) {
        streampos pos = out.tellg();
        out.read((char*)&p, sizeof(Paciente));
        if (p.id == pacienteModificado.id) {
            out.seekp(pos);
            out.write((char*)&pacienteModificado, sizeof(Paciente));
            out.close();
            return true;
        }
    }
    out.close();
    return false;
}
void listarPacientes_BIN() {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) { cout << "No se pudo abrir pacientes.bin\n"; return; }
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Paciente p{};
    cout << "\n--- LISTA DE PACIENTES ---\n";
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (p.activo) {
            cout << "ID: " << p.id << ", Nombre: " << p.nombre << " " << p.apellido
                 << ", Cédula: " << p.cedula << ", Edad: " << p.edad
                 << ", Consultas: " << p.cantidadConsultas << "\n";
        }
    }
    archivo.close();
}
bool crearPaciente_BIN(const char* nombre, const char* apellido, const char* cedula, int edad, char sexo, const char* telefono, const char* email, const char* direccion) {
    if (!validarCedula(cedula) || !validarEmail(email)) return false;
    if (buscarPacientePorCedula_BIN(cedula).id != 0) return false;
    Paciente p{};
    strncpy(p.nombre, nombre, sizeof(p.nombre)-1);
    strncpy(p.apellido, apellido, sizeof(p.apellido)-1);
    strncpy(p.cedula, cedula, sizeof(p.cedula)-1);
    p.edad = edad;
    p.sexo = sexo;
    strncpy(p.telefono, telefono, sizeof(p.telefono)-1);
    strncpy(p.email, email, sizeof(p.email)-1);
    strncpy(p.direccion, direccion, sizeof(p.direccion)-1);
    p.activo = true;
    bool ok = agregarPaciente(p);
    if (ok) { hospital.nextPacienteID++; hospital.cantidadPacientesActivos++; guardarMetadatos(); }
    return ok;
}
Paciente* buscarPacientesPorNombre_BIN(const char* nombre, int* cantidad) {
    *cantidad = 0;
    ifstream file("pacientes.bin", ios::binary);
    if (!file.is_open()) return nullptr;
    ArchivoHeader header{};
    file.read((char*)&header, sizeof(ArchivoHeader));
    Paciente temp{};
    while (file.read(reinterpret_cast<char*>(&temp), sizeof(Paciente))) {
        if (temp.activo && contieneSubcadena(temp.nombre, nombre)) (*cantidad)++;
    }
    file.close();
    if (*cantidad == 0) return nullptr;

    Paciente* resultados = new Paciente[*cantidad];
    int index = 0;
    file.open("pacientes.bin", ios::binary);
    if (!file.is_open()) { delete[] resultados; *cantidad = 0; return nullptr; }
    file.read((char*)&header, sizeof(ArchivoHeader));
    while (file.read(reinterpret_cast<char*>(&temp), sizeof(Paciente))) {
        if (temp.activo && contieneSubcadena(temp.nombre, nombre)) {
            resultados[index++] = temp;
            if (index >= *cantidad) break;
        }
    }
    file.close();
    return resultados;
}
void buscarPacientesPorNombreParcial_BIN(const char* nombreParcial) {
    int cantidad = 0;
    Paciente* resultados = buscarPacientesPorNombre_BIN(nombreParcial, &cantidad);
    cout << "\n--- Pacientes que coinciden con: " << nombreParcial << " ---\n";
    if (!resultados) { cout << "No se encontraron.\n"; return; }
    for (int i = 0; i < cantidad; i++) {
        auto& p = resultados[i];
        cout << "ID: " << p.id << " | " << p.nombre << " " << p.apellido << " | CI: " << p.cedula << " | Edad: " << p.edad << "\n";
    }
    delete[] resultados;
}

// ---------------- Doctores.bin ----------------
Doctor buscarDoctorPorCedula_BIN(const char* cedula) {
    Doctor d{};
    ifstream file("doctores.bin", ios::binary);
    if (!file.is_open()) return d;
    ArchivoHeader header{};
    file.read((char*)&header, sizeof(ArchivoHeader));
    while (file.read(reinterpret_cast<char*>(&d), sizeof(Doctor))) {
        if (d.activo && strcasecmp_compat(d.cedula, cedula) == 0) return d;
    }
    return Doctor{};
}
Doctor buscarDoctorPorID_BIN(int id) {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) return Doctor{};
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Doctor d{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (d.id == id) return d;
    }
    return Doctor{};
}
bool agregarDoctor(const Doctor& nuevoDoctor) {
    const char* archivoBin = "doctores.bin";
    ifstream chk(archivoBin, ios::binary);
    if (!chk.is_open()) inicializarArchivo(archivoBin);

    ArchivoHeader header = leerHeader(archivoBin);
    Doctor d = nuevoDoctor;
    d.id = header.proximoID;
    d.activo = true;

    fstream archivo(archivoBin, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    archivo.write((char*)&d, sizeof(Doctor));
    archivo.close();

    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(archivoBin, header);
    return true;
}
bool modificarDoctor_BIN(const Doctor& doctorModificado) {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) return false;
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();

    fstream out("doctores.bin", ios::binary | ios::in | ios::out);
    if (!out.is_open()) return false;
    Doctor d{};
    out.seekg(sizeof(ArchivoHeader));
    for (int i = 0; i < header.cantidadRegistros; i++) {
        streampos pos = out.tellg();
        out.read((char*)&d, sizeof(Doctor));
        if (d.id == doctorModificado.id) {
            out.seekp(pos);
            out.write((char*)&doctorModificado, sizeof(Doctor));
            out.close();
            return true;
        }
    }
    out.close();
    return false;
}
void listarDoctores_BIN() {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) { cout << "No se pudo abrir doctores.bin\n"; return; }
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Doctor d{};
    cout << "\n--- LISTA DE DOCTORES ---\n";
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (d.activo) {
            cout << "ID: " << d.id << ", " << d.nombre << " " << d.apellido
                 << ", Esp: " << d.especialidad << ", Tel: " << d.telefono
                 << ", Costo: $" << fixed << setprecision(2) << d.costo << "\n";
        }
    }
    archivo.close();
}
bool crearDoctor_BIN(const char* nombre, const char* apellido, const char* cedula, const char* especialidad, int experiencia, float costo) {
    if (!validarCedula(cedula)) return false;
    if (buscarDoctorPorCedula_BIN(cedula).id != 0) return false;

    Doctor d{};
    strncpy(d.nombre, nombre, sizeof(d.nombre)-1);
    strncpy(d.apellido, apellido, sizeof(d.apellido)-1);
    strncpy(d.cedula, cedula, sizeof(d.cedula)-1);
    strncpy(d.especialidad, especialidad, sizeof(d.especialidad)-1);
    d.experiencia = experiencia;
    d.costo = costo;
    d.activo = true;

    bool ok = agregarDoctor(d);
    if (ok) { hospital.nextDoctorID++; hospital.cantidadDoctoresActivos++; guardarMetadatos(); }
    return ok;
}
bool verificarCitasPendientes_BIN(int idDoctor); // forward

bool eliminarDoctor_BIN(int idDoctor) {
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);
    if (doctor.id == 0 || !doctor.activo) {
        cout << "Doctor con ID " << idDoctor << " no encontrado o ya inactivo.\n";
        return false;
    }
    if (verificarCitasPendientes_BIN(idDoctor)) {
        cout << "No se puede eliminar: el doctor tiene citas pendientes.\n";
        return false;
    }
    doctor.activo = false;
    if (!modificarDoctor_BIN(doctor)) {
        cerr << "ERROR: No se pudo sobrescribir el registro del doctor.\n";
        return false;
    }
    if (hospital.cantidadDoctoresActivos > 0) hospital.cantidadDoctoresActivos--;
    guardarMetadatos();
    cout << "Doctor " << idDoctor << " eliminado lógicamente.\n";
    return true;
}
void buscarDoctoresPorEspecialidad_BIN(const char* especialidad) {
    ifstream archivo("doctores.bin", ios::binary);
    if (!archivo.is_open()) { cout << "No se pudo abrir doctores.bin\n"; return; }
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Doctor d{};
    cout << "\n--- Doctores de especialidad: " << especialidad << " ---\n";
    bool encontrado = false;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (d.activo && contieneSubcadena(d.especialidad, especialidad)) {
            cout << "ID: " << d.id << " | " << d.nombre << " " << d.apellido
                 << " | Costo: $" << fixed << setprecision(2) << d.costo << "\n";
            encontrado = true;
        }
    }
    if (!encontrado) cout << "No se encontraron.\n";
    archivo.close();
}

// ---------------- Citas.bin ----------------
Cita buscarCitaPorID(int idCita) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return Cita{};
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Cita c{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (c.id == idCita) return c;
    }
    return Cita{};
}
bool modificarCita_BIN(const Cita& citaModificada) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return false;
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();

    fstream out("citas.bin", ios::binary | ios::in | ios::out);
    if (!out.is_open()) return false;
    Cita c{};
    out.seekg(sizeof(ArchivoHeader));
    for (int i = 0; i < header.cantidadRegistros; i++) {
        streampos pos = out.tellg();
        out.read((char*)&c, sizeof(Cita));
        if (c.id == citaModificada.id) {
            out.seekp(pos);
            out.write((char*)&citaModificada, sizeof(Cita));
            out.close();
            return true;
        }
    }
    out.close();
    return false;
}
bool verificarConflictoCita_BIN(int idDoctor, const char* fecha, const char* hora) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return false; // si no hay archivo, no hay conflicto
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Cita c{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (c.doctorID == idDoctor && c.activa && !c.completada &&
            strcasecmp_compat(c.fecha, fecha) == 0 &&
            strcasecmp_compat(c.hora, hora) == 0) {
            return true;
        }
    }
    return false;
}
bool verificarCitasPendientes_BIN(int idDoctor) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return false;
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Cita c{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (c.doctorID == idDoctor && c.activa && !c.completada) return true;
    }
    return false;
}
bool agregarCita(const Cita& nuevaCita) {
    const char* archivoBin = "citas.bin";
    ifstream chk(archivoBin, ios::binary);
    if (!chk.is_open()) inicializarArchivo(archivoBin);

    ArchivoHeader header = leerHeader(archivoBin);
    Cita c = nuevaCita;
    c.id = header.proximoID;
    c.activa = true;
    c.completada = false;

    fstream archivo(archivoBin, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    archivo.write((char*)&c, sizeof(Cita));
    archivo.close();

    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(archivoBin, header);
    return true;
}
Cita buscarCitaPendiente_BIN(int idPaciente, const char* fecha) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return Cita{};
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Cita c{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (c.pacienteID == idPaciente && c.activa && !c.completada && strcasecmp_compat(c.fecha, fecha) == 0) {
            return c;
        }
    }
    return Cita{};
}
void listarCitas_BIN() {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) { cout << "No se pudo abrir citas.bin\n"; return; }
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    Cita c{};
    cout << "\n--- LISTA DE CITAS ---\n";
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        cout << "ID: " << c.id << ", Paciente: " << c.pacienteID
             << ", Doctor: " << c.doctorID << ", Fecha: " << c.fecha
             << ", Hora: " << c.hora << ", Estado: "
             << (c.completada ? "ATENDIDA" : (c.activa ? "PENDIENTE" : "CANCELADA")) << "\n";
    }
    archivo.close();
}

// ---------------- Historiales.bin ----------------
HistorialMedico buscarHistorialPorID(int idConsulta) {
    ifstream archivo("historiales.bin", ios::binary);
    if (!archivo.is_open()) return HistorialMedico{};
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    HistorialMedico h{};
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&h, sizeof(HistorialMedico));
        if (h.idConsulta == idConsulta) return h;
    }
    return HistorialMedico{};
}
int buscarUltimaConsultaIDPorPaciente(int pacienteID, int primerID) {
    if (primerID == 0) return 0;
    int current = primerID;
    while (true) {
        HistorialMedico h = buscarHistorialPorID(current);
        if (h.idConsulta == 0 || h.siguienteConsultaID == 0) break;
        current = h.siguienteConsultaID;
    }
    return current;
}
bool modificarHistorial_BIN(const HistorialMedico& hMod) {
    ifstream archivo("historiales.bin", ios::binary);
    if (!archivo.is_open()) return false;
    ArchivoHeader header{};
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();

    fstream out("historiales.bin", ios::binary | ios::in | ios::out);
    if (!out.is_open()) return false;
    HistorialMedico h{};
    out.seekg(sizeof(ArchivoHeader));
    for (int i = 0; i < header.cantidadRegistros; i++) {
        streampos pos = out.tellg();
        out.read((char*)&h, sizeof(HistorialMedico));
        if (h.idConsulta == hMod.idConsulta) {
            out.seekp(pos);
            out.write((char*)&hMod, sizeof(HistorialMedico));
            out.close();
            return true;
        }
    }
    out.close();
    return false;
}
bool agregarHistorial_BIN(int idPaciente, HistorialMedico h) {
    const char* archivoBin = "historiales.bin";
    ifstream chk(archivoBin, ios::binary);
    if (!chk.is_open()) inicializarArchivo(archivoBin);

    ArchivoHeader header = leerHeader(archivoBin);
    h.idConsulta = header.proximoID;
    h.pacienteID = idPaciente;
    h.eliminado = false;

    fstream archivo(archivoBin, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    archivo.write((char*)&h, sizeof(HistorialMedico));
    archivo.close();

    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(archivoBin, header);

    // Vincular en paciente como lista enlazada
    Paciente p = buscarPacientePorID_BIN(idPaciente);
    if (p.id != 0) {
        if (p.primerConsultaID == 0) {
            p.primerConsultaID = h.idConsulta;
        } else {
            int ultimo = buscarUltimaConsultaIDPorPaciente(idPaciente, p.primerConsultaID);
            if (ultimo != 0) {
                HistorialMedico hl = buscarHistorialPorID(ultimo);
                hl.siguienteConsultaID = h.idConsulta;
                modificarHistorial_BIN(hl);
            }
        }
        p.cantidadConsultas++;
        actualizarPaciente(p);
    }
    return true;
}
void mostrarHistorial_BIN(int idPaciente) {
    Paciente p = buscarPacientePorID_BIN(idPaciente);
    if (p.id == 0) { cout << "Paciente no encontrado\n"; return; }
    if (p.primerConsultaID == 0) { cout << "El paciente no tiene historial.\n"; return; }

    cout << "\n--- HISTORIAL MEDICO DE " << p.nombre << " " << p.apellido << " ---\n";
    cout << "+--------------------------------------------------------------------------+\n";
    cout << "| ID | Fecha      | Hora  | Diagnostico                      | Doctor | $" << "\n";
    cout << "+--------------------------------------------------------------------------+\n";
    int current = p.primerConsultaID;
    int mostrados = 0;
    while (current != 0 && mostrados < p.cantidadConsultas) {
        HistorialMedico h = buscarHistorialPorID(current);
        if (h.idConsulta == 0) break;
        cout << "| " << setw(3) << h.idConsulta << " | " << setw(10) << h.fecha
             << " | " << setw(5) << h.hora << " | " << setw(28) << left << h.diagnostico
             << " | " << setw(6) << h.idDoctor << " | " << fixed << setprecision(2) << h.costo << "\n";
        current = h.siguienteConsultaID;
        mostrados++;
    }
    cout << "+--------------------------------------------------------------------------+\n";
}

// ---------------- Operaciones de negocio ----------------
bool modificarPaciente_BIN(const Paciente& p) { return actualizarPaciente(p); }

void agendarCita_BIN(int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {
    Paciente paciente = buscarPacientePorID_BIN(idPaciente);
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);
    if (paciente.id == 0 || !paciente.activo || doctor.id == 0 || !doctor.activo) {
        cout << "Error: El doctor o el paciente no existen o están inactivos.\n";
        return;
    }
    if (!validarHora(hora)) { cout << "Hora inválida.\n"; return; }
    if (verificarConflictoCita_BIN(idDoctor, fecha, hora)) {
        cout << "Error: Ya existe una cita pendiente para el Dr. " << idDoctor << " en esa fecha y hora.\n";
        return;
    }

    Cita nuevaCita{};
    nuevaCita.pacienteID = idPaciente;
    nuevaCita.doctorID = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha)-1);
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora)-1);
    strncpy(nuevaCita.motivo, motivo, sizeof(nuevaCita.motivo)-1);
    nuevaCita.activa = true;
    nuevaCita.completada = false;

    if (!agregarCita(nuevaCita)) {
        cout << "ERROR: No se pudo escribir la cita en citas.bin.\n";
        return;
    }

    // Obtener ID recién asignado para vincularlo al paciente
    ArchivoHeader hCitas = leerHeader("citas.bin");
    int idAsignado = hCitas.proximoID - 1;

    Paciente p = paciente;
    if (p.cantidadCitas < 20) {
        p.citasIDs[p.cantidadCitas++] = idAsignado;
        modificarPaciente_BIN(p);
    } else {
        cout << "Advertencia: Límite de 20 citas por paciente alcanzado. La cita se guardó pero no se vinculó al array interno.\n";
    }

    hospital.nextCitaID = hCitas.proximoID; // sincronizar
    hospital.cantidadCitasActivas++;
    guardarMetadatos();

    cout << "Cita agendada con éxito. ID de Cita: " << idAsignado << "\n";
}

bool cancelarCita_BIN(int idCita) {
    Cita cita = buscarCitaPorID(idCita);
    if (cita.id == 0 || !cita.activa || cita.completada) {
        cout << "Error: Cita no encontrada, inactiva o ya atendida.\n";
        return false;
    }
    cita.activa = false;
    cita.completada = false;
    if (!modificarCita_BIN(cita)) {
        cerr << "ERROR: No se pudo actualizar la cita.\n";
        return false;
    }

    Paciente paciente = buscarPacientePorID_BIN(cita.pacienteID);
    if (paciente.id != 0) {
        int index = -1;
        for (int i = 0; i < paciente.cantidadCitas; i++) {
            if (paciente.citasIDs[i] == idCita) { index = i; break; }
        }
        if (index != -1) {
            for (int i = index; i < paciente.cantidadCitas - 1; i++) {
                paciente.citasIDs[i] = paciente.citasIDs[i + 1];
            }
            paciente.cantidadCitas--;
            modificarPaciente_BIN(paciente);
        }
    }

    cout << "Cita ID " << idCita << " cancelada.\n";
    return true;
}

bool atenderCita_BIN(int idCita, const char* diagnostico, float costo) {
    Cita cita = buscarCitaPorID(idCita);
    if (cita.id == 0) { cout << "Cita no encontrada\n"; return false; }
    if (!cita.activa || cita.completada) { cout << "Cita no está pendiente\n"; return false; }

    cita.completada = true;
    cita.activa = false;
    if (!modificarCita_BIN(cita)) { cout << "ERROR: no se pudo actualizar cita\n"; return false; }

    Paciente paciente = buscarPacientePorID_BIN(cita.pacienteID);
    if (paciente.id == 0) { cout << "Paciente no encontrado para registrar historial\n"; return false; }

    HistorialMedico h{};
    strncpy(h.fecha, cita.fecha, sizeof(h.fecha)-1);
    strncpy(h.hora, cita.hora, sizeof(h.hora)-1);
    strncpy(h.diagnostico, diagnostico, sizeof(h.diagnostico)-1);
    h.idDoctor = cita.doctorID;
    h.pacienteID = cita.pacienteID;
    h.costo = costo;

    if (!agregarHistorial_BIN(paciente.id, h)) {
        cout << "ERROR: No se pudo registrar el historial.\n";
        return false;
    }

    cout << "Cita atendida y registrada en historial.\n";
    return true;
}

void listarCitasPorPaciente_BIN(int idPaciente) {
    Paciente paciente = buscarPacientePorID_BIN(idPaciente);
    if (paciente.id == 0 || !paciente.activo) {
        cout << "Error: Paciente no encontrado o inactivo.\n";
        return;
    }

    cout << "\n--- Citas del paciente " << paciente.nombre << " " << paciente.apellido
         << " (ID " << idPaciente << ") ---\n";

    if (paciente.cantidadCitas == 0) {
        cout << "Este paciente no tiene citas agendadas.\n";
        return;
    }

    for (int i = 0; i < paciente.cantidadCitas; i++) {
        int idCita = paciente.citasIDs[i];
        Cita cita = buscarCitaPorID(idCita);
        const char* estado = cita.completada ? "ATENDIDA" : (cita.activa ? "PENDIENTE" : "CANCELADA");
        if (cita.id == idCita) {
            cout << "- Cita ID: " << cita.id
                 << ", Doctor ID: " << cita.doctorID
                 << ", Fecha: " << cita.fecha
                 << ", Hora: " << cita.hora
                 << ", Motivo: " << cita.motivo
                 << " (" << estado << ")\n";
        } else {
            cout << "- [ADVERTENCIA] ID " << idCita << " registrado pero no encontrado en citas.bin.\n";
        }
    }
}

// ---------------- Interfaz de usuario (main) ----------------
int main() {
    if (!inicializarSistema()) {
        cerr << "No se pudo inicializar el sistema. Terminando programa." << endl;
        return 1;
    }

    int opcion;
    do {
        cout << "\n+--------------------------------------+\n";
        cout << "|      MENU PRINCIPAL DEL HOSPITAL     |\n";
        cout << "+--------------------------------------+\n";
        cout << "| 1. Registrar paciente                |\n";
        cout << "| 2. Registrar doctor                  |\n";
        cout << "| 3. Agendar cita                      |\n";
        cout << "| 4. Atender cita                      |\n";
        cout << "| 5. Mostrar historial de paciente     |\n";
        cout << "| 6. Cancelar cita                     |\n";
        cout << "| 7. Listar doctores                   |\n";
        cout << "| 8. Listar pacientes                  |\n";
        cout << "| 9. Buscar paciente por cédula        |\n";
        cout << "| 10. Buscar doctor por especialidad   |\n";
        cout << "| 11. Buscar pacientes por nombre parcial|\n";
        cout << "| 12. Listar citas por paciente        |\n";
        cout << "| 13. Eliminar doctor (Lógico)         |\n";
        cout << "| 0. Salir                             |\n";
        cout << "+--------------------------------------+\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada inválida. Intente de nuevo.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (opcion) {
            case 1: {
                char nombre[50], apellido[50], cedula[20], tipoSangre[5];
                char telefono[20], direccion[100], email[50];
                int edad; char sexo;
                cout << "Nombre: "; cin.getline(nombre, 50);
                cout << "Apellido: "; cin.getline(apellido, 50);
                cout << "Cedula: "; cin.getline(cedula, 20);
                cout << "Tipo de sangre: "; cin.getline(tipoSangre, 5);
                cout << "Telefono: "; cin.getline(telefono, 20);
                cout << "Dirección: "; cin.getline(direccion, 100);
                cout << "Email: "; cin.getline(email, 50);
                cout << "Edad: "; cin >> edad;
                cout << "Sexo (M/F): "; cin >> sexo;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (crearPaciente_BIN(nombre, apellido, cedula, edad, sexo, telefono, email, direccion)) {
                    cout << "Paciente registrado con ID: " << hospital.nextPacienteID - 1 << "\n";
                } else {
                    cout << "ERROR: No se pudo registrar el paciente.\n";
                }
                break;
            }
            case 2: {
                char nombre[50], apellido[50], cedula[20], especialidad[50];
                char horarioAtencion[50], telefono[20], email[50];
                int experiencia; float costo;
                cout << "Nombre: "; cin.getline(nombre, 50);
                cout << "Apellido: "; cin.getline(apellido, 50);
                cout << "Cedula profesional: "; cin.getline(cedula, 20);
                cout << "Especialidad: "; cin.getline(especialidad, 50);
                cout << "Horario de atencion: "; cin.getline(horarioAtencion, 50);
                cout << "Telefono: "; cin.getline(telefono, 20);
                cout << "Email: "; cin.getline(email, 50);
                cout << "Tiempo de experiencia: "; cin >> experiencia;
                cout << "Costo de consulta: "; cin >> costo;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (crearDoctor_BIN(nombre, apellido, cedula, especialidad, experiencia, costo)) {
                    cout << "Doctor registrado con ID: " << hospital.nextDoctorID - 1 << "\n";
                } else {
                    cout << "ERROR: No se pudo registrar el doctor.\n";
                }
                break;
            }
            case 3: {
                int idPaciente, idDoctor;
                char fecha[11], hora[6], motivo[100];
                cout << "ID del paciente: "; cin >> idPaciente;
                cout << "ID del doctor: "; cin >> idDoctor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Fecha (dd/mm/yyyy): "; cin.getline(fecha, 11);
                cout << "Hora (hh:mm): "; cin.getline(hora, 6);
                cout << "Motivo de la cita: "; cin.getline(motivo, 100);
                agendarCita_BIN(idPaciente, idDoctor, fecha, hora, motivo);
                break;
            }
            case 4: {
                int idCita; char diagnostico[200]; float costo;
                cout << "ID de la cita a atender: "; cin >> idCita;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Diagnóstico: "; cin.getline(diagnostico, 200);
                cout << "Costo final: "; cin >> costo;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (atenderCita_BIN(idCita, diagnostico, costo)) {
                    cout << "Cita ID " << idCita << " atendida.\n";
                } else {
                    cout << "Fallo al atender la cita.\n";
                }
                break;
            }
            case 5: {
                int idPaciente;
                cout << "Ingrese el ID del paciente: ";
                cin >> idPaciente;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                mostrarHistorial_BIN(idPaciente);
                break;
            }
            case 6: {
                int idCancelar;
                cout << "Ingrese el ID de la cita que desea cancelar: ";
                cin >> idCancelar;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (cancelarCita_BIN(idCancelar)) {
                    cout << "Cita cancelada correctamente.\n";
                } else {
                    cout << "No se pudo cancelar la cita.\n";
                }
                break;
            }
            case 7: { listarDoctores_BIN(); break; }
            case 8: { listarPacientes_BIN(); break; }
            case 9: {
                char cedula[20];
                cout << "Ingrese la cédula del paciente: ";
                cin.getline(cedula, 20);
                Paciente paciente = buscarPacientePorCedula_BIN(cedula);
                if (paciente.id != 0) {
                    cout << "Paciente encontrado:\n";
                    cout << "ID: " << paciente.id << ", Nombre: " << paciente.nombre << " " << paciente.apellido << ", Edad: " << paciente.edad << "\n";
                } else {
                    cout << "Paciente no encontrado o inactivo.\n";
                }
                break;
            }
            case 10: {
                char especialidad[50];
                cout << "Ingrese la especialidad a buscar: ";
                cin.getline(especialidad, 50);
                buscarDoctoresPorEspecialidad_BIN(especialidad);
                break;
            }
            case 11: {
                char nombreParcial[50];
                cout << "Ingrese parte del nombre del paciente: ";
                cin.getline(nombreParcial, 50);
                buscarPacientesPorNombreParcial_BIN(nombreParcial);
                break;
            }
            case 12: {
                int idPaciente;
                cout << "Ingrese el ID del paciente: ";
                cin >> idPaciente;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                listarCitasPorPaciente_BIN(idPaciente);
                break;
            }
            case 13: {
                int idDoctor;
                cout << "ID del doctor a eliminar (lógicamente): ";
                cin >> idDoctor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (!eliminarDoctor_BIN(idDoctor)) {
                    cout << "No se pudo eliminar el doctor.\n";
                }
                break;
            }
            case 0: cout << "Gracias por usar el sistema del hospital.\n"; break;
            default: cout << "Opción no válida. Intente de nuevo.\n"; break;
        }
    } while (opcion != 0);

    cerrarSistema();
    return 0;
}






