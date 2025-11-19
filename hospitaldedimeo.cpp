#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;   

// Compatibility prototype for case-insensitive compare (defined later)
int strcasecmp_compat(const char* a, const char* b);

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
    int pacienteID;
    int siguienteConsultaID; // Para listas enlazadas en archivo (avanzado)

    bool activo = true;
};

// Estructura Paciente
struct Paciente {
    int id = 0;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];


    int citasIDs[20]; 
    int cantidadCitas = 0;

    int primerConsultaID; 

    char alergias[500];
    char observaciones[500];

    bool activo = true;
};

// Estructura Doctor
struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];

    int doctoresID [50];
    int cantidadPacientes;
    int capacidadPacientes;

    int citasagendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool activo = true;
};

// Estructura Cita
struct Cita {
    int id = 0;
    int pacienteID;
    int doctorID;
    char fecha[11];
    char hora[6];
    char motivo[100];
    bool completada = false;
    bool activa = true;
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    
    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;

    int totalPacientesRegistrados; // Reemplaza a cantidadPacientes
    int totalDoctoresRegistrados; // Reemplaza a cantidadDoctores
    int totalCitasAgendadas;
    int totalConsultasRealizadas;

    //gestion del paciente
};
// Esta estructura se almacenará en RAM y se sincronizará con "hospital.bin"
struct HospitalMetadata {
    // Contadores para asignar el SIGUIENTE ID (reemplaza a siguienteIdPaciente, etc.)
    int nextPacienteID = 1; 
    int nextDoctorID = 1; 
    int nextCitaID = 1; 
    int nextHistorialID = 1;
    
    // Contadores de registros activos (reemplaza a cantidadPacientes, etc.)
    int cantidadPacientesActivos = 0;
    int cantidadDoctoresActivos = 0;
    int cantidadCitasActivas = 0;
    // (Puedes agregar campos para nombre, dirección, etc. si lo necesitas)
};

// Variable global para mantener el estado del sistema en memoria
HospitalMetadata hospital;

struct ArchivoHeader {
    int cantidadRegistros;      // Total de registros escritos
    int proximoID;              // Siguiente ID a asignar
    int registrosActivos;       // Registros con eliminado == false
    int version;                // Para control de formato (ej: 1)
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
    ArchivoHeader header;
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return header; 
    
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return header;
}

// Actualiza el header (SOBREESCRIBE el inicio del archivo)
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    // Abre en modo IN/OUT para no borrar el archivo
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.seekp(0, ios::beg); // Posicionarse al inicio
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}
Paciente leerPacientePorIndice(int indice) {
    Paciente p;
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) return p; // Devuelve paciente vacío

    // 1. Calcular posición exacta
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // 2. Ir directamente a esa posición (¡Magia!)
    archivo.seekg(posicion);
    
    // 3. Leer SOLO ese registro
    archivo.read((char*)&p, sizeof(Paciente));
    
    archivo.close();
    return p;
}

int buscarIndiceDeID(int id) {
    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) return -1;
    
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); // Lee el header
    
    Paciente temp;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Paciente));
        if (temp.id == id && !temp.eliminado) {
            archivo.close();
            return i; // ¡Encontrado! Este es el índice
        }
    }
    archivo.close();
    return -1; // No encontrado
}

Paciente buscarPacientePorCedula_BIN(const char* cedula) {
    Paciente p;
    p.id = 0; // Marcar como no encontrado por defecto
    
    // Abrir archivo para lectura
    std::ifstream file("pacientes.bin", std::ios::binary); 
    
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir pacientes.bin para lectura." << endl;
        return p;
    }
    while (file.read(reinterpret_cast<char*>(&p), sizeof(Paciente))) {
        
        if (p.activo && strcasecmp_compat(p.cedula, cedula) == 0) {
            file.close();
            return p; 
        }
    }

    file.close();
    return p; // Retorna el Paciente no encontrado (ID=0)
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
Paciente* buscarPacientesPorNombre_BIN(const char* nombre, int* cantidad) {
    *cantidad = 0;
    Paciente tempPaciente; 
    
    // 1. Abrir el archivo para la PRIMERA PASADA (Conteo)
    std::ifstream file("pacientes.bin", std::ios::binary); 
    
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir pacientes.bin para lectura." << endl;
        return nullptr;
    }
    
    // 1.1. Primera Pasada: Contar coincidencias
    while (file.read(reinterpret_cast<char*>(&tempPaciente), sizeof(Paciente))) {
        // Verificar si está activo Y si el nombre coincide parcialmente
        if (tempPaciente.activo && contieneSubcadena(tempPaciente.nombre, nombre)) {
            (*cantidad)++;
        }
    }
    
    // 1.2. Cerrar y verificar
    file.close(); 
    
    if (*cantidad == 0) {
        return nullptr; // No se encontró nada
    }

    Paciente* resultados = new Paciente[*cantidad];
    int index = 0;

    file.open("pacientes.bin", std::ios::binary); 

    if (!file.is_open()) {
        delete[] resultados;
        *cantidad = 0;
        return nullptr; 
    }
    
    while (file.read(reinterpret_cast<char*>(&tempPaciente), sizeof(Paciente))) {
        if (tempPaciente.activo && contieneSubcadena(tempPaciente.nombre, nombre)) {
            resultados[index++] = tempPaciente; 
            
            if (index >= *cantidad) break; 
        }
    }
    
    file.close();
    return resultados;
}
bool agregarPaciente(Paciente nuevoPaciente) {
    const char* archivoBin = "pacientes.bin";
    
    // 1. Leer header para obtener ID y contadores
    ArchivoHeader header = leerHeader(archivoBin);
    
    // 2. Asignar metadata
    nuevoPaciente.id = header.proximoID;
    nuevoPaciente.eliminado = false;
    nuevoPaciente.fechaCreacion = time(NULL);
    nuevoPaciente.fechaModificacion = time(NULL);
    // ... inicializar sus arrays fijos (citasIDs)
    
    // 3. Abrir en modo 'append' (añadir al final)
    fstream archivo(archivoBin, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    
    // 4. Escribir el nuevo registro
    archivo.write((char*)&nuevoPaciente, sizeof(Paciente));
    archivo.close();
    
    // 5. Actualizar y guardar el header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(archivoBin, header);
    
    return true;
}
bool validarCedula(const char* cedula);
bool actualizarPaciente(Paciente pacienteModificado) {
    // 1. Buscar el ÍNDICE (posición) del paciente
    int indice = buscarIndiceDeID(pacienteModificado.id);
    if (indice == -1) return false; // No existe

    // 2. Calcular la posición en BYTES
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // 3. Abrir en modo IN/OUT (para sobrescribir)
    fstream archivo("pacientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    // 4. Actualizar metadata
    pacienteModificado.fechaModificacion = time(NULL);

    // 5. Posicionarse para ESCRIBIR
    archivo.seekp(posicion);
    
    // 6. Sobrescribir el registro completo
    archivo.write((char*)&pacienteModificado, sizeof(Paciente));
    
    archivo.close();
    return true;
}

bool eliminarPaciente(int id) {
    // 1. Buscar el ÍNDICE
    int indice = buscarIndiceDeID(id);
    if (indice == -1) return false;

    // 2. LEER el paciente en esa posición
    Paciente p = leerPacientePorIndice(indice);
    
    // 3. Marcar como eliminado
    p.eliminado = true;
    
    // 4. ACTUALIZAR (sobrescribir) el registro en el archivo
    if (!actualizarPaciente(p)) return false; // Usa la función que ya hiciste

    // 5. Actualizar el header
    ArchivoHeader header = leerHeader("pacientes.bin");
    header.registrosActivos--;
    actualizarHeader("pacientes.bin", header);
    
    return true;
}
void listarTodosPacientes() {
    cout << "\n+---------------------------------------------------------------+\n";
    cout << "|                    LISTA DE PACIENTES                        |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "| ID  | NOMBRE COMPLETO      | CEDULA        | EDAD | CONSULTAS |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";


   ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) return;

    // 1. Leer header para saber cuántos hay
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    cout << "Listando " << header.registrosActivos << " pacientes...\n";
    
    Paciente temp; // UNA sola variable temporal
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // 2. Leer un registro a la vez
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // 3. Si no está eliminado, mostrarlo
        if (!temp.eliminado) {
            cout << "ID: " << temp.id << ", Nombre: " << temp.nombre << endl;
            // ... imprimir resto de datos
        }
        // 4. En la siguiente iteración, 'temp' se sobrescribe.
        //    (No se acumula en memoria)
    }
    archivo.close();
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
HistorialMedico* obtenerHistorialCompleto_BIN(int pacienteID, int* cantidad) {
    *cantidad = 0;
    
    // 1. Obtener la información inicial del paciente
    Paciente paciente = buscarPacientePorID(pacienteID);

    if (paciente.id == 0 || paciente.cantidadConsultas == 0) {
        return nullptr;
    }

    *cantidad = paciente.cantidadConsultas;
    
    // 2. Asignar la memoria en RAM para los resultados que se devolverán
    HistorialMedico* resultados = new HistorialMedico[*cantidad];
    int index = 0;
    int currentID = paciente.primerConsultaID; 
    
    // 3. Recorrer la lista enlazada en el archivo
    while (currentID != 0 && index < *cantidad) {
        // Cargar el registro desde el archivo
        HistorialMedico consulta = buscarHistorialPorID(currentID);
        
        if (consulta.idConsulta == currentID && consulta.activo) {
            // Guardar en el array de resultados
            resultados[index++] = consulta; 
            // Avanzar al siguiente ID de la lista enlazada
            currentID = consulta.siguienteConsultaID; 
        } else {
            // Si hay un error de lectura o el registro está inactivo/corrupto, detenerse
            cerr << "Advertencia: Error de enlace o registro inactivo en historial. ID: " << currentID << endl;
            break;
        }
    }
    
    return resultados;
}
bool atenderCita_BIN(int idPaciente, const char* fecha, float costo) {
    // 1. Buscar la cita pendiente en el archivo citas.bin
    Cita citaEncontrada = buscarCitaPendiente_BIN(idPaciente, fecha);

    if (citaEncontrada.id == 0) {
        cout << "ERROR: Cita pendiente para el paciente " << idPaciente << " en " << fecha << " no encontrada." << endl;
        return false;
    }

    // 2. Modificar el estado de la cita en la RAM
    citaEncontrada.completada = true; // Marcar como completada/atendida
    // Opcionalmente: citaEncontrada.activa = false;
    
    // 3. Sobreescribir el registro de Cita en el archivo citas.bin
    if (!modificarCita_BIN(citaEncontrada)) {
        cout << "ERROR: No se pudo actualizar el registro de la cita en el disco." << endl;
        return false;
    }
    
    // 4. Crear el nuevo registro de Historial Médico
    HistorialMedico nuevoHistorial;
    // El ID se asigna en la función de historial
    nuevoHistorial.pacienteID = idPaciente; 
    strcpy(nuevoHistorial.fecha, citaEncontrada.fecha);
    strcpy(nuevoHistorial.hora, citaEncontrada.hora);
    // En el código original, el diagnóstico era el motivo de la cita.
    strcpy(nuevoHistorial.diagnostico, citaEncontrada.motivo); 
    nuevoHistorial.idDoctor = citaEncontrada.doctorID; 
    nuevoHistorial.costo = costo; // Costo viene como parámetro
    nuevoHistorial.activo = true;
    
    // 5. Agregar la consulta al historial (utilizando la lógica de enlace creada previamente)
    
    // NOTA: Esta función DEBE existir y manejar la asignación de nextHistorialID, 
    // la escritura en historiales.bin y el enlace de la lista del paciente.
    // La firma es simplificada a:
    // void agregarConsultaAlHistorial_BIN(int pacienteID, const HistorialMedico& consulta);
    
    // Usamos la función del paso anterior para manejar el archivo de historial.
    agregarHistorial_BIN(idPaciente, nuevoHistorial); 

    cout << "Cita (" << citaEncontrada.id << ") marcada como atendida y historial actualizado para el paciente " << idPaciente << endl;
    return true;
}

void mostrarHistorial_BIN(int idPaciente) {
    // 1. Obtener el paciente para verificación
    Paciente paciente = buscarPacientePorID(idPaciente);

    if (paciente.id == 0 || !paciente.activo) {
        cout << "Error: Paciente con ID " << idPaciente << " no encontrado o inactivo." << endl;
        return;
    }
    
    int cantidad = 0;
    // 2. Cargar el historial completo a la RAM (Array temporal)
    HistorialMedico* historialCompleto = obtenerHistorialCompleto_BIN(idPaciente, &cantidad);

    if (cantidad == 0 || historialCompleto == nullptr) {
        cout << "El paciente " << paciente.nombre << " no tiene historial médico registrado." << endl;
        return;
    }

    // 3. Imprimir el encabezado
    cout << "\n--- HISTORIAL MEDICO DEL PACIENTE: " << paciente.nombre << " " << paciente.apellido << " ---" << endl;
    cout << "+---------------------------------------------------------------------------------------------+" << endl;
    cout << "| ID |    Fecha      |  Hora  |        Diagnostico      | ID Doctor |   Costo   |" << endl;
    cout << "+---------------------------------------------------------------------------------------------+" << endl;

    // 4. Iterar sobre el array temporal cargado desde el archivo
    for (int i = 0; i < cantidad; i++) {
        HistorialMedico& h = historialCompleto[i];
        
        // Usamos setw() para asegurar el formato de tabla
        cout << "| " << setw(2) << h.idConsulta
             << " | " << setw(11) << h.fecha
             << " | " << setw(6) << h.hora
             << " | " << setw(23) << h.diagnostico
             << " | " << setw(9) << h.idDoctor
             << " | $" << fixed << setprecision(2) << setw(7) << h.costo << " |" << endl;
    }

    cout << "+---------------------------------------------------------------------------------------------+" << endl;
    
    // 5. ¡CRUCIAL! Liberar la memoria del array temporal
    delete[] historialCompleto; 
}



HistorialMedico* obtenerUltimaConsulta(Paciente* paciente) {
    if (paciente->cantidadConsultas == 0) return nullptr;
    return &paciente->historial[paciente->cantidadConsultas - 1];
}
//
Doctor* buscarDoctorPorCedula(Hospital* hospital, const char* cedula);
void redimensionarArrayDoctores(Hospital* hospital);
// Prototypes for functions defined later but used earlier
void redimensionarCitas(Hospital* hospital, int nuevaCapacidad);

// Simple case-insensitive strcmp replacement for portability (windows/msvc)
int strcasecmp_compat(const char* a, const char* b) {
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca = ca + ('a' - 'A');
        if (cb >= 'A' && cb <= 'Z') cb = cb + ('a' - 'A');
        if (ca != cb) return (unsigned char)ca - (unsigned char)cb;
        ++a; ++b;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

bool crearDoctor_BIN(const char* nombre, const char* apellido, const char* cedula,
                     const char* especialidad, int aniosExperiencia, float costoConsulta) 
{
    // 1. Verificación de unicidad por cédula (Búsqueda secuencial en el archivo)
    Doctor existente = buscarDoctorPorCedula_BIN(cedula);
    if (existente.id != 0) {
        cout << "Error: Ya existe un doctor registrado con la cédula " << cedula << endl;
        return false;
    }

    // 2. Crear y llenar el objeto Doctor temporal en la RAM
    Doctor nuevo;
    
    // Asignar el ID usando el contador global
    nuevo.id = hospital.nextDoctorID; 
    
    // Copiar los datos (Usando strncpy es más seguro que strcpy)
    strncpy(nuevo.nombre, nombre, 50);
    strncpy(nuevo.apellido, apellido, 50);
    strncpy(nuevo.cedula, cedula, 20);
    strncpy(nuevo.especialidad, especialidad, 50);
    nuevo.aniosExperiencia = aniosExperiencia;
    nuevo.costoConsulta = costoConsulta;
    
    // Inicializar campos fijos (reemplaza a la asignación dinámica original)
    // Estos campos deberían ser ahora IDs de relación o arrays fijos.
    nuevo.activo = true; 
    
    // 3. Escribir el nuevo registro al final del archivo doctores.bin
    std::fstream file("doctores.bin", std::ios::out | std::ios::binary | std::ios::app); 
    
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&nuevo), sizeof(Doctor));
        file.close();

        // 4. Actualizar los metadatos globales en RAM y en Disco
        hospital.nextDoctorID++;
        hospital.cantidadDoctoresActivos++; // Asumiendo que existe este contador
        guardarMetadatos();
        
        cout << "Doctor " << nuevo.id << " (" << nuevo.nombre << ") registrado con éxito en disco." << endl;
        return true;
    } else {
        cerr << "ERROR: No se pudo abrir/crear doctores.bin para escritura." << endl;
        return false;
    }
}
Doctor buscarDoctorPorCedula_BIN(const char* cedula) {
    Doctor d;
    d.id = 0;
    
    std::ifstream file("doctores.bin", std::ios::binary); 
    
    if (!file.is_open()) return d;

    // Bucle de lectura secuencial: lee un Doctor en la RAM temporalmente en cada iteración
    while (file.read(reinterpret_cast<char*>(&d), sizeof(Doctor))) {
        // Verificar que esté activo Y que la cédula coincida
        if (d.activo && strcmp(d.cedula, cedula) == 0) {
            file.close();
            return d; 
        }
    }

    file.close();
    d.id = 0; // Si sale del bucle sin encontrar, marcar como no encontrado
    return d; 
}

Doctor* buscarDoctoresPorEspecialidad_BIN(const char* especialidad, int* cantidad) {
    *cantidad = 0;
    Doctor tempDoctor; 
    
    // 1. Primera Pasada: Contar coincidencias
    std::ifstream file("doctores.bin", std::ios::binary); 
    if (!file.is_open()) return nullptr;
    
    while (file.read(reinterpret_cast<char*>(&tempDoctor), sizeof(Doctor))) {
        // Verificar si está activo Y si la especialidad coincide
        if (tempDoctor.activo && strcmp(tempDoctor.especialidad, especialidad) == 0) {
            (*cantidad)++;
        }
    }
    file.close(); 
    
    if (*cantidad == 0) return nullptr;

    // 2. Asignación de Memoria para los Resultados (Temporal en RAM)
    Doctor* resultados = new Doctor[*cantidad];
    int index = 0;

    // 3. Segunda Pasada: Recolección de datos
    file.open("doctores.bin", std::ios::binary); 
    if (!file.is_open()) {
        delete[] resultados;
        *cantidad = 0;
        return nullptr; 
    }
    
    while (file.read(reinterpret_cast<char*>(&tempDoctor), sizeof(Doctor))) {
        if (tempDoctor.activo && strcmp(tempDoctor.especialidad, especialidad) == 0) {
            // Copiar el objeto completo leído al array de resultados
            resultados[index++] = tempDoctor; 
            if (index >= *cantidad) break; 
        }
    }
    
    file.close();
    return resultados; // Devuelve el array temporal que el llamador debe liberar
}
bool asignarPacienteADoctor_BIN(int idDoctor, int idPaciente) {
    // 1. Cargar el Doctor del archivo
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);

    if (doctor.id == 0 || !doctor.activo) {
        cout << "Error: Doctor no encontrado." << endl;
        return false;
    }
    
    // 2. Verificar si ya está asignado (Recorrido en RAM, solo 20 elementos)
    for (int i = 0; i < doctor.cantidadPacientesAsignados; i++) {
        if (doctor.pacientesAsignados[i] == idPaciente) {
            cout << "El paciente ya está asignado a este doctor." << endl;
            return false;
        }
    }
    
    // 3. Verificar límite de capacidad (fija)
    if (doctor.cantidadPacientesAsignados >= 20) { 
        cout << "Error: El doctor ha alcanzado su límite de 20 pacientes asignados." << endl;
        return false;
    }

    // 4. Asignar el paciente en RAM
    doctor.pacientesAsignados[doctor.cantidadPacientesAsignados++] = idPaciente;

    // 5. Sobrescribir el registro de Doctor en el archivo doctores.bin
    if (modificarDoctor_BIN(doctor)) {
        cout << "Paciente " << idPaciente << " asignado al Doctor " << idDoctor << "." << endl;
        return true;
    } else {
        cout << "Error al guardar la asignación del doctor en el disco." << endl;
        return false;
    }
}

void listarPacientesDeDoctor_BIN(int idDoctor) {
    // 1. Cargar el registro del Doctor (Acceso Aleatorio)
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);

    if (doctor.id == 0 || !doctor.activo) {
        cout << "Error: Doctor no encontrado." << endl;
        return;
    }
    
    cout << "\nPacientes asignados al Dr. " << doctor.nombre << " " << doctor.apellido << " (ID: " << idDoctor << "):" << endl;

    if (doctor.cantidadPacientesAsignados == 0) {
        cout << "No hay pacientes asignados actualmente." << endl;
        return;
    }
    
    // 2. Iterar sobre los IDs de pacientes
    for (int i = 0; i < doctor.cantidadPacientesAsignados; i++) {
        int idPaciente = doctor.pacientesAsignados[i];
        
        // 3. Buscar y cargar la información del paciente (Carga Bajo Demanda/Acceso Aleatorio)
        Paciente p = buscarPacientePorID_BIN(idPaciente); 

        if (p.id != 0 && p.activo) {
            // 4. Mostrar la información
            cout << "- " << p.nombre << " " << p.apellido << " (Cédula: " << p.cedula << ", ID: " << p.id << ")" << endl;
        } else {
            // Manejar si el paciente estaba asignado pero fue eliminado lógicamente
            cout << "- [ADVERTENCIA] Paciente ID: " << idPaciente << " está en la lista pero fue eliminado." << endl;
        }
    }
}

bool removerPacienteDeDoctor_BIN(int idDoctor, int idPaciente) {
    // 1. Cargar el Doctor del archivo
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);

    if (doctor.id == 0 || !doctor.activo) return false;

    // 2. Buscar índice del paciente en RAM
    int index = -1;
    for (int i = 0; i < doctor.cantidadPacientesAsignados; i++) {
        if (doctor.pacientesAsignados[i] == idPaciente) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        cout << "El paciente " << idPaciente << " no está asignado al doctor " << idDoctor << "." << endl;
        return false;
    }

    // 3. Remover el paciente del array en RAM (compactando la lista)
    for (int i = index; i < doctor.cantidadPacientesAsignados - 1; i++) {
        doctor.pacientesAsignados[i] = doctor.pacientesAsignados[i + 1];
    }
    doctor.cantidadPacientesAsignados--;
    
    // 4. Sobrescribir el registro de Doctor en el archivo doctores.bin
    if (modificarDoctor_BIN(doctor)) {
        cout << "Paciente " << idPaciente << " removido del Doctor " << idDoctor << "." << endl;
        return true;
    } else {
        cout << "Error al guardar la remoción del paciente en el disco." << endl;
        return false;
    }
}

void listarDoctores_BIN() {
    Doctor d;
    bool encontrado = false;

    // 1. Abrir el archivo de doctores para lectura binaria
    std::ifstream file("doctores.bin", std::ios::binary); 
    
    if (!file.is_open()) {
        cout << "El archivo de doctores no existe o no se pudo abrir." << endl;
        return;
    }

    // 2. Imprimir encabezados
    cout << "\n+-------------------------------------------------------------------+" << endl;
    cout << "|                       LISTA DE DOCTORES ACTIVOS                     |" << endl;
    cout << "+-----+------------------------+----------------+---------------------+" << endl;
    cout << "| ID  | NOMBRE COMPLETO        | CÉDULA         | ESPECIALIDAD        |" << endl;
    cout << "+-----+------------------------+----------------+---------------------+" << endl;

    // 3. Lectura Secuencial: Iterar sobre el archivo binario
    while (file.read(reinterpret_cast<char*>(&d), sizeof(Doctor))) {
        // Verificar si el registro está activo (Borrado Lógico)
        if (d.activo) {
            // Imprimir cada doctor con formato alineado
            cout << "| " << setw(4) << d.id << " | "
                 << setw(22) << left << d.nombre << " " // Se asume que solo d.nombre se muestra en la columna
                 << " | " 
                 << setw(14) << d.cedula << " | "
                 << setw(19) << d.especialidad << " |" << endl;
            encontrado = true;
        }
    }
    
    // 4. Cerrar el archivo
    file.close(); 

    // 5. Mensaje de resultado
    if (!encontrado) {
        cout << "|                  No hay doctores activos registrados.               |" << endl;
    }

    cout << "+-----+------------------------+----------------+---------------------+" << endl;
}


bool eliminarDoctor_BIN(int idDoctor) {
    // 1. Cargar el registro del Doctor (Acceso Aleatorio)
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);

    if (doctor.id == 0 || !doctor.activo) {
        cout << "Doctor con ID " << idDoctor << " no encontrado o ya inactivo." << endl;
        return false;
    }
    
    // 2. Verificar Citas Pendientes (Acceso Secuencial a citas.bin)
    if (verificarCitasPendientes_BIN(idDoctor)) {
        cout << "No se puede eliminar al Doctor " << idDoctor << ": el doctor tiene citas pendientes." << endl;
        return false;
    }
    
    // 3. Implementar la Eliminación LÓGICA en RAM
    doctor.activo = false;
    // Opcional: Podrías limpiar sus pacientesAsignados y citasAgendadas si usas la lista fija

    // 4. Sobrescribir el registro de Doctor en el archivo doctores.bin
    if (modificarDoctor_BIN(doctor)) {
        // 5. Actualizar Metadatos Globales
        hospital.cantidadDoctoresActivos--; 
        guardarMetadatos();
        
        cout << "Doctor " << idDoctor << " (" << doctor.nombre << ") eliminado LÓGICAMENTE del sistema." << endl;
        return true;
    } else {
        cerr << "ERROR: No se pudo sobrescribir el registro del doctor en el disco." << endl;
        return false;
    }
}

//Gestion de citas
void agendarCita_BIN(int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {

    // 1. Verificar si el doctor y el paciente existen y están activos (Carga Bajo Demanda/Acceso Aleatorio)
    if (buscarDoctorPorID_BIN(idDoctor).id == 0 || buscarPacientePorID_BIN(idPaciente).id == 0) {
        cout << "Error: El doctor o el paciente no existen o están inactivos." << endl;
        return;
    }

    // 2. Verificar conflictos de horario (Acceso Secuencial a citas.bin)
    if (verificarConflictoCita_BIN(idDoctor, fecha, hora)) {
        cout << "Error: Ya existe una cita pendiente para el Dr. " << idDoctor << " en esa fecha y hora." << endl;
        return;
    }
    
    // 3. Crear y llenar el objeto Cita temporal en la RAM
    Cita nuevaCita;
    nuevaCita.id = hospital.nextCitaID;
    nuevaCita.pacienteID = idPaciente;
    nuevaCita.doctorID = idDoctor;
    
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha));
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora));
    strncpy(nuevaCita.motivo, motivo, sizeof(nuevaCita.motivo)); // Asumimos que el motivo se recibe o se pide
    
    // Flags de estado
    nuevaCita.activa = true;
    nuevaCita.completada = false;

    // 4. Escribir el nuevo registro al final del archivo citas.bin
    std::fstream file("citas.bin", std::ios::out | std::ios::binary | std::ios::app); 
    
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&nuevaCita), sizeof(Cita));
        file.close();

        // 5. Actualizar la lista de IDs de citas en el registro del Paciente
        // Se asume que el paciente tiene un array fijo de citas (ej: citasIDs[20])
        Paciente paciente = buscarPacientePorID_BIN(idPaciente);
        if (paciente.cantidadCitas < 20) { // Límite del array fijo
            paciente.citasIDs[paciente.cantidadCitas++] = nuevaCita.id;
            modificarPaciente_BIN(paciente);
        } else {
            cout << "Advertencia: No se pudo agregar la cita al array interno del paciente (límite alcanzado)." << endl;
            // La cita sigue guardada en citas.bin.
        }
        
        // 6. Actualizar y guardar los metadatos globales
        hospital.nextCitaID++;
        hospital.cantidadCitasActivas++; 
        guardarMetadatos();
        
        cout << "Cita agendada con éxito. ID de Cita: " << nuevaCita.id << endl;
    } else {
        cerr << "ERROR: No se pudo abrir/crear citas.bin para escritura." << endl;
    }
}

bool cancelarCita_BIN(int idCita) {
    // 1. Cargar el registro de Cita (Acceso Aleatorio)
    Cita cita = buscarCitaPorID(idCita);

    if (cita.id == 0 || !cita.activa || cita.completada) {
        cout << "Error: Cita con ID " << idCita << " no encontrada, inactiva o ya atendida." << endl;
        return false;
    }
    
    // 2. Implementar la Cancelación LÓGICA en RAM
    // Marcamos la cita como inactiva (eliminación lógica) y no completada.
    cita.activa = false; 
    cita.completada = false; 

    // 3. Sobrescribir el registro de Cita en el archivo citas.bin
    if (!modificarCita_BIN(cita)) {
        cerr << "ERROR: No se pudo sobrescribir el registro de la cita en el disco." << endl;
        return false;
    }
    
    // 4. Actualizar el registro del Paciente (Limpieza del array de IDs de citas)
    Paciente paciente = buscarPacientePorID_BIN(cita.pacienteID);
    
    if (paciente.id != 0) {
        int index = -1;
        // Buscar el ID de la cita en el array fijo del paciente
        for (int i = 0; i < paciente.cantidadCitas; i++) {
            if (paciente.citasIDs[i] == idCita) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            // Remover el ID del array interno del paciente (compactación)
            for (int i = index; i < paciente.cantidadCitas - 1; i++) {
                paciente.citasIDs[i] = paciente.citasIDs[i + 1];
            }
            paciente.cantidadCitas--;
            modificarPaciente_BIN(paciente);
        }
    }

    cout << "Cita ID " << idCita << " cancelada LÓGICAMENTE." << endl;
    return true;
}


void listarCitasPorPaciente_BIN(int idPaciente) {
    // 1. Cargar el registro del Paciente (ya no pide el ID, lo recibe)
    Paciente paciente = buscarPacientePorID_BIN(idPaciente);

    if (paciente.id == 0 || !paciente.activo) {
        // Muestra un error al usuario sobre el resultado de la búsqueda
        cout << "Error: Paciente con ID " << idPaciente << " no encontrado o inactivo." << endl;
        return;
    }
    
    // ... el resto de la lógica de lectura de archivos ...
}
void listarCitasPorPaciente_BIN(int idPaciente) {
    // 1. Cargar el registro del Paciente (Acceso Aleatorio)
    Paciente paciente = buscarPacientePorID_BIN(idPaciente);

    if (paciente.id == 0 || !paciente.activo) {
        // La función de servicio informa sobre el error de datos.
        cout << "Error: Paciente con ID " << idPaciente << " no encontrado o inactivo." << endl;
        return;
    }

    // La función de servicio se encarga de formatear y mostrar el resultado.
    cout << "\n--- Citas Agendadas del Paciente " << paciente.nombre << " " << paciente.apellido << " (ID " << idPaciente << ") ---" << endl;

    if (paciente.cantidadCitas == 0) {
        cout << "Este paciente no tiene citas agendadas." << endl;
        return;
    }

    // 2. Iterar sobre la lista de IDs de citas en el registro del paciente (en RAM)
    for (int i = 0; i < paciente.cantidadCitas; i++) {
        int idCita = paciente.citasIDs[i];
        
        // 3. Cargar la Cita individualmente (Carga Bajo Demanda/Acceso Aleatorio)
        Cita cita = buscarCitaPorID(idCita); 

        // 4. Verificar y mostrar la información
        if (cita.id == idCita) {
            
            // Determinar el estado basado en los flags booleanos
            const char* estado;
            if (cita.completada) {
                estado = "ATENDIDA";
            } else if (cita.activa) {
                estado = "PENDIENTE";
            } else {
                estado = "CANCELADA"; 
            }

            cout << "- Cita ID: " << cita.id 
                 << ", Doctor ID: " << cita.doctorID
                 << ", Fecha: " << cita.fecha
                 << ", Hora: " << cita.hora
                 << ", Motivo: " << cita.motivo
                 << " (Estado: " << estado << ")" << endl;
        } else {
            cout << "- [ADVERTENCIA] El ID de Cita " << idCita << " está registrado pero no se encontró el detalle en el archivo." << endl;
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


void cerrarSistema() {
    // 1. Guardar los contadores y flags actualizados en hospital.bin
    guardarMetadatos();
    
    cout << "Metadatos guardados con éxito. El sistema está listo para cerrarse." << endl;
}
bool inicializarSistema(); 
void cerrarSistema();      
Paciente buscarPacientePorID_BIN(int id);
Doctor buscarDoctorPorID_BIN(int id);
bool crearPaciente_BIN(const char* nombre, const char* apellido, const char* cedula, int edad, char sexo, const char* telefono, const char* email, const char* direccion);
bool crearDoctor_BIN(const char* nombre, const char* apellido, const char* cedula, const char* especialidad, int experiencia, float costo);
void listarDoctores_BIN();
void listarPacientes_BIN();
void agendarCita_BIN(int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo);
bool cancelarCita_BIN(int idCita);
bool eliminarDoctor_BIN(int idDoctor);
Paciente buscarPacientePorCedula_BIN(const char* cedula);
void buscarDoctoresPorEspecialidad_BIN(const char* especialidad);
void buscarPacientesPorNombreParcial_BIN(const char* nombreParcial);
void mostrarHistorial_BIN(int idPaciente);
bool atenderCita_BIN(int idCita, const char* diagnostico, float costo); 
void listarCitasPorPaciente_BIN(int idPaciente);

// Implementación simple de inicialización y cierre para que main funcione
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
// =================================================================

int main() {
    // ❌ ELIMINADO: Se quita la instanciación de Hospital y la memoria dinámica
    // Hospital hospital;
    // hospital.pacientes = new Paciente[...];
    // ...

    // Inicializa el sistema (carga metadatos del disco)
    if (!inicializarSistema()) {
        cerr << "No se pudo inicializar el sistema. Terminando programa." << endl;
        return 1;
    }

    int opcion;
    do {
        // Mostrar el menú (Usamos el menú original)
        cout << "\n+--------------------------------------+\n";
        cout << "|      MENU PRINCIPAL DEL HOSPITAL     |\n";
        cout << "+--------------------------------------+\n";
        cout << "| 1. Registrar paciente                |\n";
        cout << "| 2. Registrar doctor                  |\n";
        cout << "| 3. Agendar cita                      |\n";
        cout << "| 4. Atender cita                      |\n";
        cout << "| 5. Mostrar historial de paciente     |\n";
        cout << "| 6. Cancelar cita                     |\n"; // Ajustado para ser el 6
        cout << "| 7. Listar doctores                   |\n"; // Ajustado
        cout << "| 8. Listar pacientes                  |\n"; // Ajustado
        cout << "| 9. Buscar paciente por cédula        |\n"; // Ajustado
        cout << "| 10. Buscar doctor por especialidad    |\n"; 
        cout << "| 11. Buscar pacientes por nombre parcial|\n"; 
        cout << "| 12. Listar citas por paciente        |\n"; // Nuevo caso (Usado en caso 12 anterior)
        cout << "| 13. Eliminar doctor (Lógico)         |\n"; // Ajustado
        cout << "| 0. Salir                             |\n";
        cout << "+--------------------------------------+\n";
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada inválida. Intente de nuevo.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar el buffer

        switch (opcion) {
            case 1: { // Registrar Paciente
                char nombre[50], apellido[50], cedula[20], tipoSangre[5];
                char telefono[20], direccion[100], email[50];
                int edad;
                char sexo;

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

                // 🔄 Llamada a la función BIN
                if (crearPaciente_BIN(nombre, apellido, cedula, edad, sexo, telefono, email, direccion)) {
                    cout << "Paciente registrado con ID: " << hospital.nextPacienteID - 1 << "\n";
                } else {
                    cout << "ERROR: No se pudo registrar el paciente (Cédula duplicada o error de disco).\n";
                }
                break;
            }
            case 2: { // Registrar Doctor
                char nombre[50], apellido[50], cedula[20], especialidad[50];
                char horarioAtencion[50], telefono[20], email[50];
                int experiencia;
                float costo;

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
                
                // 🔄 Llamada a la función BIN
                if (crearDoctor_BIN(nombre, apellido, cedula, especialidad, experiencia, costo)) {
                    cout << "Doctor registrado con ID: " << hospital.nextDoctorID - 1 << "\n";
                } else {
                    cout << "ERROR: No se pudo registrar el doctor (Cédula duplicada o error de disco).\n";
                }
                break;
            }
            case 3: { // Agendar Cita
                int idPaciente, idDoctor;
                char fecha[11], hora[6], motivo[100];

                cout << "ID del paciente: "; cin >> idPaciente;
                cout << "ID del doctor: "; cin >> idDoctor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Fecha (dd/mm/yyyy): "; cin.getline(fecha, 11);
                cout << "Hora (hh:mm): "; cin.getline(hora, 6);
                cout << "Motivo de la cita: "; cin.getline(motivo, 100);

                // 🔄 Llamada a la función BIN
                agendarCita_BIN(idPaciente, idDoctor, fecha, hora, motivo);
                break;
            }
            case 4: { // Atender cita
                int idCita;
                char diagnostico[100];
                float costo;
                
                cout << "ID de la cita a atender: "; cin >> idCita;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Diagnóstico: "; cin.getline(diagnostico, 100);
                cout << "Costo final: "; cin >> costo;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                // 🔄 Llamada a la función BIN
                if (atenderCita_BIN(idCita, diagnostico, costo)) {
                    cout << "Cita ID " << idCita << " atendida y registrada en el historial del paciente.\n";
                } else {
                    cout << "Fallo al atender la cita (ID no encontrado, ya atendida o error de disco).\n";
                }
                break;
            }

            case 5: { // Mostrar Historial Médico
                int idPaciente;
                cout << "Ingrese el ID del paciente: ";
                cin >> idPaciente;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                // 🔄 Llamada a la función BIN
                mostrarHistorial_BIN(idPaciente); 
                break;
            }
            case 6: { // Cancelar Cita (Mapeado del switch original case 6)
                int idCancelar;
                cout << "Ingrese el ID de la cita que desea cancelar: ";
                cin >> idCancelar;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                // 🔄 Llamada a la función BIN
                if (cancelarCita_BIN(idCancelar)) {
                    cout << "Cita cancelada correctamente.\n";
                } else {
                    cout << "No se pudo cancelar la cita (ID no encontrado o ya cancelada/atendida).\n";
                }
                break;
            }
            case 7: { // Listar Doctores
                // 🔄 Llamada a la función BIN
                listarDoctores_BIN();
                break;
            }
            case 8: { // Listar Pacientes
                // 🔄 Llamada a la función BIN
                listarPacientes_BIN();
                break;
            }
            case 9: { // Buscar paciente por cédula
                char cedula[20];
                cout << "Ingrese la cédula del paciente: ";
                cin.getline(cedula, 20);
                
                // 🔄 Llamada a la función BIN
                Paciente paciente = buscarPacientePorCedula_BIN(cedula);
                if (paciente.id != 0) {
                    cout << "Paciente encontrado:\n";
                    cout << "ID: " << paciente.id << ", Nombre: " << paciente.nombre << " " << paciente.apellido << ", Edad: " << paciente.edad << "\n";
                } 
                else {
                    cout << "Paciente no encontrado o inactivo.\n";
                }
                break;
            }
            case 10: { // Buscar doctor por especialidad
                char especialidad[50];
                cout << "Ingrese la especialidad a buscar: ";
                cin.getline(especialidad, 50);

                // 🔄 Llamada a la función BIN
                buscarDoctoresPorEspecialidad_BIN(especialidad);
                break;
            }
            case 11: { // Buscar pacientes por nombre parcial
                char nombreParcial[50];
                cout << "Ingrese parte del nombre del paciente: ";
                cin.getline(nombreParcial, 50);
                
                // 🔄 Llamada a la función BIN
                buscarPacientesPorNombreParcial_BIN(nombreParcial);
                break;
            }
            case 12: { // Listar Citas por Paciente (Mapeado del caso anterior)
                int idPaciente;
                cout << "Ingrese el ID del paciente: ";
                cin >> idPaciente;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                // 🔄 Llamada a la función BIN
                listarCitasPorPaciente_BIN(idPaciente); 
                break;
            }
            case 13: { // Eliminar doctor (Lógico)
                int idDoctor;
                cout << "ID del doctor a eliminar (lógicamente): ";
                cin >> idDoctor;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                // 🔄 Llamada a la función BIN
                if (eliminarDoctor_BIN(idDoctor)) {
                    cout << "Doctor eliminado lógicamente del sistema.\n";
                } else {
                    cout << "Fallo al eliminar doctor (No encontrado o tiene citas pendientes).\n";
                }
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

    // 🔄 Llama a la nueva función que guarda los metadatos en disco y no libera punteros grandes
    cerrarSistema();

    return 0;
}





