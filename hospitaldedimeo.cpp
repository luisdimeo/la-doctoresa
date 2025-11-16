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
void buscarPacientesPorNombreParcial_BIN(const char* nombreParcial) {
    Paciente p;
    bool encontrado = false;

    // 1. Abrir el archivo de pacientes para lectura binaria
    std::ifstream file("pacientes.bin", std::ios::binary); 
    
    if (!file.is_open()) {
        cout << "El archivo de pacientes no existe o no se pudo abrir." << endl;
        return;
    }

    // 2. Imprimir encabezados de la tabla
    cout << "\n+---------------------------------------------------------------+" << endl;
    cout << "|             PACIENTES QUE COINCIDEN CON EL NOMBRE             |" << endl;
    cout << "+-----+----------------------+---------------+------+-----------+" << endl;
    cout << "| ID  | NOMBRE COMPLETO      | CÉDULA        | EDAD | CONSULTAS |" << endl;
    cout << "+-----+----------------------+---------------+------+-----------+" << endl;

    // 3. Lectura Secuencial: Iterar sobre el archivo binario
    while (file.read(reinterpret_cast<char*>(&p), sizeof(Paciente))) {
        // Verificar dos condiciones:
        // a) El registro debe estar activo (borrado lógico)
        // b) El nombre debe contener el patrón parcial
        if (p.activo && contieneTexto(p.nombre, nombreParcial)) {
            // 4. Mostrar el registro encontrado (Temporalmente en RAM)
            cout << "| " << setw(3) << p.id << " | "
                 << setw(20) << left << p.nombre << " "
                 << setw(15) << p.apellido << " | "
                 << setw(13) << p.cedula << " | "
                 << setw(4) << p.edad << " | "
                 << setw(9) << p.cantidadConsultas << " |" << endl; // Se usa cantidadConsultas
            encontrado = true;
        }
    }
    
    // 5. Cerrar el archivo
    file.close(); 

    // 6. Mensaje de resultado
    if (!encontrado) {
        cout << "| No se encontraron pacientes activos con el nombre parcial '" << setw(37) << left << nombreParcial << " |" << endl;
    }

    cout << "+-----+----------------------+---------------+------+-----------+" << endl;
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
    cout << "\n+-------------------------------------------------------------------+\n";
    cout << "|                       LISTA DE DOCTORES                             |\n";
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

    // Ensure capacity
    if (hospital->cantidadCitas >= hospital->capacidadCitas) {
        redimensionarCitas(hospital, hospital->capacidadCitas * 2);
    }

    // Check for conflicts
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& cita = hospital->citas[i];
        if (cita.idDoctor == idDoctor && strcmp(cita.fecha, fecha) == 0 && strcmp(cita.hora, hora) == 0) {
            cout << " Ya existe una cita con el doctor en esa fecha y hora.\n";
            return;
        }
    }

    // Create and append
    Cita nuevaCita;
    nuevaCita.id = hospital->siguienteIdCita++;
    nuevaCita.idPaciente = idPaciente;
    nuevaCita.idDoctor = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha));
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora));
    strcpy(nuevaCita.motivo, "");
    strcpy(nuevaCita.estado, "pendiente");
    nuevaCita.atendida = false;

    hospital->citas[hospital->cantidadCitas++] = nuevaCita;
    cout << "Cita agendada con ID: " << nuevaCita.id << "\n";

}

bool cancelarCita(Hospital* hospital, int idCita) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita) {
            strcpy(hospital->citas[i].estado, "cancelada");
            return true;
        }
    }
    return false;
    
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
void redimensionarCitas(Hospital* hospital, int nuevaCapacidad) {
    Cita* nuevoArreglo = new Cita[nuevaCapacidad];

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        nuevoArreglo[i] = hospital->citas[i];
    }

    delete[] hospital->citas;
    hospital->citas = nuevoArreglo;
    hospital->capacidadCitas = nuevaCapacidad;
}

void destruirHospital(Hospital* hospital) {
    // Liberar memoria de cada paciente
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        delete[] hospital->pacientes[i].citasAgendadas;   
        delete[] hospital->pacientes[i].historial;        
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
    cout << "| 10. buscar paciente parciales        |\n";
    cout << "|11. eliminar paciente                 |\n";
    cout << "|12. eliminar doctor                   |\n";
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
            char telefono[20], direccion[100], email[50];
            int edad;
            char sexo;
            Paciente nuevo;
            nuevo.id = hospital.cantidadPacientes + 1;
            cout << "Nombre: "; cin.getline(nombre, 50);
            cout << "Apellido: "; cin.getline(apellido, 50);
            cout << "Cedula: "; cin.getline(cedula, 20);
            cout << "Tipo de sangre: "; cin.getline(tipoSangre, 5);
            cout << "Telefono: "; cin.getline(telefono, 20);
            cout << "Dirección: "; cin.getline(direccion, 100);
            cout << "Email: "; cin.getline(email, 50);
            cout << "Edad: "; cin >> edad;
            cout << "Sexo (M/F): "; cin >> sexo;
            cin.ignore();
            crearPaciente(&hospital, nombre, apellido, cedula, edad, sexo, telefono, email, direccion);
            nuevo.capacidadHistorial = 0;
            hospital.pacientes[hospital.cantidadPacientes] = nuevo;
            hospital.cantidadPacientes++;
            cout << "Paciente registrado con ID: " << nuevo.id << "\n";
            break;
        }
        case 2: {
            char nombre[50], apellido[50], cedula[20], especialidad[50];
            char horarioAtencion[50], telefono[20], email[50];
            int experiencia;
            float costo;
            Doctor nuevo;
            nuevo.id = hospital.cantidadDoctores + 1;
            cout << "Nombre: "; cin.getline(nombre, 50);
            cout << "Apellido: "; cin.getline(apellido, 50);
            cout << "Cedula profesional: "; cin.getline(cedula, 20);
            cout << "Especialidad: "; cin.getline(especialidad, 50);
            cout << "Horario de atencion: "; cin.getline(horarioAtencion, 50);
            cout << "Telefono: "; cin.getline(telefono, 20);
            cout << "Email: "; cin.getline(email, 50);
            cout << "Tiempo de experiencia: "; cin >> experiencia;
            cout << "Costo de consulta: "; cin >> costo;
            cin.ignore(); 
            crearDoctor(&hospital, nombre, apellido, cedula, especialidad, experiencia, costo);
            hospital.doctores[hospital.cantidadDoctores] = nuevo;
            hospital.cantidadDoctores++;
            cout << "Doctor registrado con ID: " << nuevo.id << "\n";
            break;
        }
        case 3: {
            int idPaciente, idDoctor;
            char fecha[11], hora[6];
            Cita nuevo;

            nuevo.id = hospital.capacidadCitas + 1;
            cout << "ID del paciente: "; cin >> idPaciente;
            cout << "ID del doctor: "; cin >> idDoctor;
            cin.ignore();
            cout << "Fecha (dd/mm/yyyy): "; 
            cin.getline(fecha, 11);
            cout << "Hora (hh:mm): "; 
            cin.getline(hora, 6);
            if (hospital.cantidadCitas >= 100) {
                cout << " No se pueden registrar mas citas.\n";
                 break;
                }
            nuevo.id = hospital.cantidadCitas + 1;
            strcpy(nuevo.fecha, fecha);
            strcpy(nuevo.hora, hora);
            nuevo.idPaciente = idPaciente;
            nuevo.idDoctor = idDoctor;
            strcpy(nuevo.estado, "pendiente");

            cout << "Motivo de la cita: "; cin.getline(nuevo.motivo, 100);
            hospital.citas[hospital.cantidadCitas++] = nuevo;
            cout << "Cita registrada con ID: " << nuevo.id << "\n";
            break;
        }
        case 4: {
            int idCita;
            cout << "ID de la cita: ";cin >> idCita;
            cin.ignore();
            Cita* cita = nullptr;
            for (int i = 0; i < hospital.cantidadCitas; i++) {
                if (hospital.citas[i].id == idCita) {
                    cita = &hospital.citas[i];
                    break;
                }
            }
            if (!cita) {
                cout << " La cita con ID " << idCita << " no existe.\n";
                break;
            }
            if (strcmp(cita->estado, "pendiente") != 0) {
                cout << " La cita no está pendiente.\n";
                break;
            }
            char diagnostico[100];
            float costo;
            cout << "Diagnostico: ";
            cin.ignore(); 
            cin.getline(diagnostico, 100);
            cout << "Costo: ";
            cin >> costo;
            cin.ignore();
            strcpy(cita->estado, "atendida");
            for (int i = 0; i < hospital.cantidadPacientes; i++) {
                if (hospital.pacientes[i].id == cita->idPaciente) {
                    Paciente* paciente = &hospital.pacientes[i];
                    
                    if (paciente->capacidadHistorial >= 10) {
                        cout << " Historial lleno. No se puede registrar.\n";
                        break;
                    }
                    HistorialMedico nuevo;
                    nuevo.idConsulta = paciente->capacidadHistorial + 1;
                    strcpy(nuevo.fecha, cita->fecha);
                    strcpy(nuevo.hora, cita->hora);
                    strcpy(nuevo.diagnostico, diagnostico);
                    nuevo.idDoctor = cita->idDoctor;
                    nuevo.costo = costo;
                    paciente->historial[paciente->capacidadHistorial++] = nuevo;
                    cout << "Cita atendida correctamente y registrada en el historial.\n";
                    break;
                }
            }
            break;
        }

        case 5: {
            int idPaciente;
            cout << "Ingrese el ID del paciente: ";
            cin >> idPaciente;
            cin.ignore();
            bool encontrado = false;
            for (int i = 0; i < hospital.cantidadPacientes; i++) {
                if (hospital.pacientes[i].id == idPaciente) {
                    mostrarHistorial(&hospital.pacientes[i]); // muestra historial directamente
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) {
                cout << "No se encontró ningún paciente con ese ID.\n";
            }
            break;
        }
        case 6:{
           int idCancelar;
           cout << "Ingrese el ID de la cita que desea cancelar: ";
           cin >> idCancelar;
           cancelarCita(&hospital, idCancelar);
        }
        case 7:
            listarDoctores(&hospital);
            break;
        case 8:
            listarPacientes(&hospital);
            break;

        case 9: {
            char cedula[20];
            cout << "Ingrese la cedula del paciente: ";
            cin >> cedula;
            Paciente* paciente = buscarPacientePorCedula(&hospital, cedula);
            if (paciente) {
                cout << "Paciente encontrado:\n";
                cout << "Nombre: " << paciente->nombre << "\n";
                cout << "Apellido: " << paciente->apellido << "\n";
                cout << "Edad: " << paciente->edad << "\n";
             } 
             else {
                cout << " Paciente no encontrado.\n";
            }
            break;
        }
        case 10: {
             char especialidad[50];
             int*cantidad;
             cout << "Ingrese la especialidad a buscar: ";
             cin.ignore();
             cin.getline(especialidad, 50);

             buscarDoctoresPorEspecialidad(&hospital, especialidad, cantidad );
             break;
        }
        case 11: {
            char nombreParcial[50];
             cout << "Ingrese parte del nombre del paciente: ";
             cin.ignore();
             cin.getline(nombreParcial, 50);
             buscarPacientesPorNombreParcial(&hospital, nombreParcial);
             break;
            }
        case 12: {
            int idPaciente;
            cout << "Ingrese el ID del paciente: ";
            cin >> idPaciente;
            cin.ignore();
            bool encontrado = false;
            for (int i = 0; i < hospital.cantidadPacientes; i++) {
                if (hospital.pacientes[i].id == idPaciente) {
                    mostrarHistorial(&hospital.pacientes[i]); 
                    encontrado = true;
                }
                 break;
                }
            }
        case 13: {
            int idDoctor;
            cout << "ID del doctor a eliminar: ";
            cin >> idDoctor;
            eliminarDoctor(&hospital, idDoctor);
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


destruirHospital(&hospital);


return 0;
}





