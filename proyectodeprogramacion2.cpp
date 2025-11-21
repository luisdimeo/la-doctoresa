#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <limits> 
#include <cctype>
using namespace std;   




//estructura de hospital la doctoresa 

// Estructura HistorialMedico
struct HistorialMedico {
    int id;
    char fecha[11];
    char hora[6];
    char diagnostico[500];
    char tratamiento[500];
    char observaciones[500];
    int idPaciente;
    double costo;

    int siguienteConsultaID;
    bool eliminado;
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
    bool activo;
    int citasIDs[20];

    
    int cantidadConsultas;
    int primerConsultaID;
    

    
    int cantidadCitas;
    int catitasIDs[20];

    char alergias[500];
    char observaciones[500];

    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// Estructura Doctor 
struct Doctor {
    int id;
    char nombre[50];
    int edad;
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];
    int pacientesIDs [50];
    int citas [30];
    bool activo;
    int citasIDs[20];

    

    int capacidadPacientes;

    
    int cantidadCitas;
   

    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// Estructura Cita
struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[10];
    

    char observaciones[200];
    int ConsultaID;
    bool confirmada;
    bool atendida;
    int consultasID;

    time_t fechaCreacion;
    time_t fechaModificacion;

    bool eliminado;
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    
    int cantidadPacientes;
    

    
    int cantidadDoctores;
    

    
    int cantidadCitas;


    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;

    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int siguienteIDConsulta;

    
};
struct ArchivoHeader {
    int cantidadRegistros;      
    int proximoID;              
    int registrosActivos;       
    int version;                
};
ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header;
    // Inicializar el header con un valor inválido por defecto (ej. -1)
    header.proximoID = -1; 
    
    // Abrir en modo de lectura binaria
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para lectura del Header: " << nombreArchivo << endl;
        return header;
    }
    
    // Leer exactamente el tamaño del Header desde el inicio (posición 0)
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    
    archivo.close();
    return header;
}
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para actualizar el Header: " << nombreArchivo << endl;
        return false;
    }
    
    // Posicionarse al inicio del archivo (byte 0) para la escritura
    archivo.seekp(0, ios::beg);
    
    // Escribir exactamente el tamaño del Header
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    
    archivo.close();
    return true;
}
long calcularPosicion(int indice, size_t tamanoRegistro) {
    long posicion = sizeof(ArchivoHeader) + (indice * tamanoRegistro);
    return posicion;
}

bool inicializarArchivo(const char* nombreArchivo) {
    
    fstream archivo(nombreArchivo, ios::binary | ios::out);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo crear o abrir el archivo para inicializar: " 
                  << nombreArchivo << endl;
        return false;
    }
    
    // 2. Definir la estructura inicial del Header.
    ArchivoHeader headerInicial = {
        0,   // cantidadRegistros: 0
        1,   // proximoID: 1 (Los IDs deben comenzar en 1)
        0,   // registrosActivos: 0
        1    // version: 1
    };
    
    // 3. Escribir el Header al inicio del archivo.
    archivo.write((char*)&headerInicial, sizeof(ArchivoHeader));
    
    archivo.close();
    return true;
}
bool agregarPaciente(Hospital* h, Paciente nuevoPaciente) {
    const char* nombreArchivo = "pacientes.bin";
    
    // 1. Asignar el ID y la metadata usando los contadores globales del Hospital
    nuevoPaciente.id = h->siguienteIdPaciente;
    nuevoPaciente.eliminado = false; // El registro está activo
    nuevoPaciente.fechaCreacion = time(NULL);
    nuevoPaciente.fechaModificacion = time(NULL);
    

    
    fstream archivo(nombreArchivo, ios::binary | ios::app);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para agregar paciente." << endl;
        return false;
    }
    
    // 3. Escribir el nuevo registro de Paciente al final del archivo.
    archivo.write((char*)&nuevoPaciente, sizeof(Paciente));
    archivo.close();
    
    // 4. Actualizar el Header del archivo (pacientes.bin).
    ArchivoHeader header = leerHeader(nombreArchivo);
    header.cantidadRegistros++;
    header.registrosActivos++;
    actualizarHeader(nombreArchivo, header);
    
    // 5. Actualizar los contadores globales del Hospital (en RAM y en disco).
    h->siguienteIdPaciente++;
    h->totalPacientesRegistrados++; // Mantenemos el total
    
    
    cout << "Paciente ID " << nuevoPaciente.id << " registrado exitosamente." << endl;
    return true;
}

int buscarIndiceDeID(int id) {
    const char* nombreArchivo = "pacientes.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de pacientes para busqueda." << endl;
        return -1;
    }

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 
    
    Paciente temp; // Una variable temporal para leer un registro a la vez
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Leer un solo registro en la variable temporal
        archivo.read((char*)&temp, sizeof(Paciente));
        
        if (temp.id == id && !temp.eliminado) {
            archivo.close();
            return i; // ¡Encontrado! Retornamos la posición (índice).
        }
    }
    
    archivo.close();
    return -1; // ID no encontrado.
}
Paciente obtenerPacientePorCedula(const char* cedula) {
    const char* nombreArchivo = "pacientes.bin";
    Paciente pEncontrado = {0}; // Estructura vacía para retornar si no se encuentra
    
    std::ifstream archivo(nombreArchivo, std::ios::binary); 
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de pacientes para busqueda por cedula." << std::endl;
        return pEncontrado;
    }
    
    // 1. Leer el Header para obtener la cantidad total de registros.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    Paciente temp; // Variable temporal para la lectura secuencial
    
    // 2. Iterar sobre todos los registros.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // 3. Comparar la cédula y verificar si el registro está activo.
        // Usar strncmp si sabes el tamaño fijo, o la funcion que uses (strcasecmp_compat).
        if (!temp.eliminado && strcmp(temp.cedula, cedula) == 0) { 
            // Si la cédula coincide y el paciente está activo
            archivo.close();
            return temp; // Devolver la estructura Paciente encontrada
        }
    }
    
    archivo.close();
    return pEncontrado; // Retornar estructura vacía si no se encuentra
}

Paciente obtenerPacientePorID(int indice) {
    Paciente p;
    
    if (indice < 0) return p; // Manejo de error si el índice es inválido

    ifstream archivo("pacientes.bin", ios::binary);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir pacientes.bin para lectura aleatoria." << endl;
        return p;
    }

    // Usar la función de bajo nivel que ya implementaste
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // Posicionarse directamente en el inicio del registro
    archivo.seekg(posicion, ios::beg);
    
    // Leer el registro completo de Paciente
    archivo.read((char*)&p, sizeof(Paciente));
    
    archivo.close();
    return p;
}

bool actualizarPaciente(Paciente pModificado) {
    const char* nombreArchivo = "pacientes.bin";
    
    // 1. Encontrar el índice (posición) actual del registro en el archivo.
    // Necesitamos este índice para calcular la posición en bytes.
    int indice = buscarIndiceDeID(pModificado.id);
    if (indice == -1) {
        cerr << "Error: Paciente con ID " << pModificado.id << " no encontrado para actualizar." << std::endl;
        return false;
    }

    // 2. Calcular la posición exacta en bytes.
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // 3. Abrir el archivo en modo de lectura y escritura binaria.
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para actualizar paciente." << endl;
        return false;
    }
    
    // 4. Actualizar la metadata de modificación.
    pModificado.fechaModificacion = time(NULL);

    // 5. Posicionar el puntero de escritura directamente en el inicio del registro.
    archivo.seekp(posicion, ios::beg);
    
    // 6. Escribir/Sobrescribir el registro completo.
    archivo.write((char*)&pModificado, sizeof(Paciente));
    
    archivo.close();
    cout << "Paciente ID " << pModificado.id << " actualizado exitosamente." << endl;
    return true;
}
    bool eliminarPaciente(Hospital* h, int id) {
    const char* nombreArchivo = "pacientes.bin";
    
    // 1. Obtener la copia del registro actual.
    // Buscar el índice real del registro dentro del archivo
    int indice = buscarIndiceDeID(id);
    if (indice == -1) {
        cerr << "Error: Paciente ID " << id << " no encontrado." << endl;
        return false;
    }

    // Leer el registro por índice
    Paciente p = obtenerPacientePorID(indice);
    if (p.id == 0 || p.eliminado) {
        std::cerr << "Error: Paciente ID " << id << " ya estaba eliminado o es inválido." << std::endl;
        return false;
    }

    // 2. Aplicar el borrado LÓGICO (la bandera) y actualizar metadata
    p.eliminado = true;
    p.fechaModificacion = time(NULL);

    // 3. Sobrescribir el registro en el archivo (Usando la función actualizarPaciente).
    if (!actualizarPaciente(p)) {
        std::cerr << "Error: No se pudo sobrescribir el registro marcado como eliminado." << std::endl;
        return false;
    }

    // 4. Actualizar el header de pacientes para decrementar registros activos
    ArchivoHeader header = leerHeader(nombreArchivo);
    if (header.registrosActivos > 0) header.registrosActivos--;
    actualizarHeader(nombreArchivo, header);

    std::cout << "Paciente ID " << id << " eliminado logicamente." << std::endl;
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


void listarTodosPacientes() {
    const char* nombreArchivo = "pacientes.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de pacientes para listado." << endl;
        return;
    }
    
    // 1. Leer el Header para obtener la cantidad total de registros.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 
    
    // Configuración de la tabla
    cout << "\n+---------------------------------------------------------------+\n";
    cout << "| " << setw(59) << left << "LISTA DE PACIENTES ACTIVOS" << " |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "| ID  | NOMBRE COMPLETO      | CEDULA        | EDAD | CONSULTAS |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";

    Paciente temp; // Variable temporal para contener UN solo registro a la vez
    int activosEncontrados = 0;
    
    // 2. Iterar sobre la cantidad TOTAL de registros en el archivo.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Leer el siguiente registro del archivo de manera secuencial.
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // 3. Comprobar que el registro no esté eliminado.
        if (!temp.eliminado) {
            // 4. Mostrar el registro activo con formato.
            cout << "| " << setw(3) << temp.id << " | "
                      << setw(20) << left << temp.nombre << " | "
                      << setw(13) << temp.cedula << " | "
                      << setw(4) << temp.edad << " | "
                      // Usamos cantidadConsultas, ya que capacidadHistorial fue eliminada.
                      << setw(9) << temp.cantidadConsultas << " |\n";
            activosEncontrados++;
        }
        
        if (archivo.fail()) break;
    }
    
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "Total de pacientes listados: " << activosEncontrados 
              << " (Activos según Header: " << header.registrosActivos << ")" << endl;
              
    archivo.close();
}

void buscarPacientesPorNombreParcial(const char* nombreParcial) {
    const char* nombreArchivo = "pacientes.bin";
    bool encontrado = false;
    
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de pacientes para busqueda parcial." << endl;
        return;
    }
    
    // Leer el Header para iterar correctamente
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    // Configuración de la tabla
    cout << "+---------------------------------------------------------------+\n";
    cout << "| " << setw(59) << left << "PACIENTES QUE COINCIDEN CON EL NOMBRE" << " |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";
    cout << "| ID  | NOMBRE COMPLETO      | CÉDULA        | EDAD | CONSULTAS |\n";
    cout << "+-----+----------------------+---------------+------+-----------+\n";

    Paciente temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // Comprobación: 1. No eliminado, 2. Nombre parcial coincide
        // Usamos strstr para buscar una subcadena.
        if (!temp.eliminado && strstr(temp.nombre, nombreParcial) != nullptr) { 
            cout << "| " << setw(3) << temp.id << " | "
                      << setw(20) << left << temp.nombre << " | "
                      << setw(13) << temp.cedula << " | "
                      << setw(4) << temp.edad << " | "
                      << setw(9) << temp.cantidadConsultas << " |\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        cout << "| " << setw(59) << left << "No se encontraron pacientes activos con ese nombre parcial." << " |\n";
    }

    cout << "+-----+----------------------+---------------+------+-----------+\n";
    archivo.close();
}

//historial medico del pasciente 
int agregarHistorialMedico(Hospital* h, HistorialMedico nuevaConsulta);



Cita leerConsultaPorID(int idBuscado) {
    Cita c;
    ifstream archivo("consultas.bin", ios::binary);
    if (!archivo.is_open()) return c;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    // Iteramos por todos los registros escritos en el archivo
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        
        // Si el ID coincide y no está eliminada, la retornamos
        if (c.id == idBuscado && !c.eliminado) {
            archivo.close();
            return c;
        }
    }
    
    // Si no se encuentra, retornamos una estructura vacía (ID 0)
    c.id = 0; 
    archivo.close();
    return c;
}
HistorialMedico obtenerHistorialMedicoPorID(int idBuscado);

bool enlazarYAgregarConsulta(Hospital* h, int pacienteID, HistorialMedico nuevaConsulta) {
    // 1. Obtener el Paciente y verificar existencia (acceso aleatorio)
    Paciente pacienteActual = obtenerPacientePorID(pacienteID);
    if (pacienteActual.id == 0) return false;

    // 2. Asignar ID y guardar la nueva consulta en el archivo historiales.bin
    nuevaConsulta.idPaciente = pacienteID;
    nuevaConsulta.siguienteConsultaID = -1; // Siempre es el último de la lista
    nuevaConsulta.eliminado = false;
    
    // La función agregarHistorialMedico guarda en disco y devuelve el ID asignado
    int nuevaConsultaID = agregarHistorialMedico(h, nuevaConsulta); 

    // 3. Lógica de Enlace (El reemplazo del 'new' y 'delete[]'):
    if (pacienteActual.primerConsultaID == 0 || pacienteActual.primerConsultaID == -1) {
        // CASO 1: Es la PRIMERA consulta del paciente.
        pacienteActual.primerConsultaID = nuevaConsultaID;
        
    } else {
        // CASO 2: Ya existe historial. Hay que buscar el ÚLTIMO nodo y actualizar su enlace.
        
        int idActual = pacienteActual.primerConsultaID;
        int idAnterior = 0;
        
        // Asumimos que existe una función para leer un registro de historial por ID
        HistorialMedico consultaActual; 

        // Recorrer la lista en disco hasta encontrar el último (siguienteConsultaID == -1)
        while (idActual != -1) {
            idAnterior = idActual;
            // Aquí se llama a la función de lectura de historial (ej: obtenerHistorialMedicoPorID)
            consultaActual = obtenerHistorialMedicoPorID(idActual); 
            idActual = consultaActual.siguienteConsultaID;
        }


        consultaActual.siguienteConsultaID = nuevaConsultaID; 
        
        

    }
    pacienteActual.cantidadConsultas++;
    actualizarPaciente(pacienteActual); 

    return true;
}
bool agregarHistorialMedico(HistorialMedico consultaActual);

void agregarHistorial(Hospital* h, int pacienteID) {
    

    HistorialMedico nuevo;
    
    

    cout << "Fecha (dd/mm/aaaa): ";
    cin.getline(nuevo.fecha, 20);

    cout << "Hora (hh:mm): ";
    cin.getline(nuevo.hora, 10);

    cout << "Diagnóstico: ";
    cin.getline(nuevo.diagnostico, 100);
    
    cout << "Tratamiento: ";
    cin.getline(nuevo.tratamiento, 500); 

    cout << "ID del doctor: ";
    cin >> nuevo.id;
    cin.ignore();

    cout << "Costo: ";
    cin >> nuevo.costo;
    cin.ignore();

    // Llamar a la función de enlace para guardarlo en el archivo historiales.bin
    if (enlazarYAgregarConsulta(h, pacienteID, nuevo)) {
        cout << "Historial agregado correctamente.\n";
    } else {
        cerr << "ERROR: No se pudo agregar el historial o el paciente no existe.\n";
    }
}


void mostrarHistorial(int pacienteID) {
    // 1. Obtener el Paciente (para obtener el primer enlace)
    Paciente paciente = obtenerPacientePorID(pacienteID); 
    
    if (paciente.id == 0) {
        std::cerr << "Error: Paciente no encontrado.\n";
        return;
    }

    if (paciente.primerConsultaID == 0 || paciente.primerConsultaID == -1) {
        std::cout << " Este paciente no tiene historial médico registrado.\n";
        return;
    }

    cout << "\nHISTORIAL MEDICO DEL PACIENTE ID: " << pacienteID << "\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";
    cout << "| ID |    Fecha    |  Hora  |      Diagnostico      | ID Doctor |   Costo  |\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";

    int idActual = paciente.primerConsultaID;
    
    // 2. Recorrer la lista en disco
    while (idActual != -1) {
        // Asumimos que existe la función obtenerHistorialMedicoPorID(int idConsulta);
        HistorialMedico h = obtenerHistorialMedicoPorID(idActual);
        
        // Mostrar datos
        cout << "| " << setw(2) << h.id 
                  << " | " << setw(11) << h.fecha
                  << " | " << setw(6) << h.hora
                  << " | " << setw(21) << std::left << h.diagnostico
                  << " | " << setw(9) << h.id
                  << " | $" << setw(7) << std::right << std::fixed << std::setprecision(2) << h.costo << " |\n";
        
        // Avanzar al siguiente nodo en el archivo
        idActual = h.siguienteConsultaID; 
    }

    std::cout << "+---------------------------------------------------------------------------------------------+\n";
}


HistorialMedico obtenerUltimaConsultaDeDisco(Paciente paciente) {
    // Estructura vacía, usada para indicar que no se encontró nada.
    HistorialMedico hVacia = {0}; 
    
    // 1. Verificar si hay historial registrado
    if (paciente.cantidadConsultas == 0 || paciente.primerConsultaID <= 0) {
        return hVacia;
    }

    int idActual = paciente.primerConsultaID;
    HistorialMedico consultaActual;

    // 2. Recorrer la lista enlazada en el archivo historiales.bin
    while (idActual != -1) {
        // Leer el registro de la consulta actual del archivo
        consultaActual = obtenerHistorialMedicoPorID(idActual); 
        
        // 3. Comprobar si este es el último nodo (enlace = -1)
        if (consultaActual.siguienteConsultaID == -1) {
            return consultaActual; // ¡Último nodo encontrado!
        }
        
        // 4. Moverse al siguiente nodo en el disco
        idActual = consultaActual.siguienteConsultaID;
        
        // Manejo de error si se encuentra un enlace roto (id 0 o loop inesperado)
        if (idActual == 0) { 
            std::cerr << "Advertencia: Se detectó un enlace roto en el historial médico." << std::endl;
            break;
        }
    }

    return hVacia; // Devolver estructura vacía si el recorrido falla
}
bool cargarDatosHospital(Hospital* h);
bool guardarDatosHospital(Hospital* h);

bool agregarDoctor(Hospital* h) {
    const char* nombreArchivo = "doctores.bin";
    Doctor nuevoDoctor;
    
    cout << "\n--- REGISTRO DE NUEVO DOCTOR ---\n";
    
    // 1. Recolección de Datos del Usuario
    cout << "Nombre completo: ";
    cin.getline(nuevoDoctor.nombre, sizeof(nuevoDoctor.nombre));

    cout << "Cédula: ";
    cin.getline(nuevoDoctor.cedula, sizeof(nuevoDoctor.cedula));
    
    cout << "Edad: ";
    if (!(cin >> nuevoDoctor.edad)) return false;
    cin.ignore();

    cout << "Teléfono: ";
    cin.getline(nuevoDoctor.telefono, sizeof(nuevoDoctor.telefono));
    
    cout << "Especialidad: ";
    cin.getline(nuevoDoctor.especialidad, sizeof(nuevoDoctor.especialidad));
    
    // 2. Asignar ID y Metadata
    nuevoDoctor.id = h->siguienteIdDoctor;
    nuevoDoctor.eliminado = false; // El registro está activo
    nuevoDoctor.fechaCreacion = time(NULL);
    nuevoDoctor.fechaModificacion = time(NULL);
    
    
    nuevoDoctor.capacidadPacientes = 0;
    nuevoDoctor.cantidadCitas = 0;

    
    memset(nuevoDoctor.pacientesIDs, 0, sizeof(nuevoDoctor.pacientesIDs));
    memset(nuevoDoctor.citas, 0, sizeof(nuevoDoctor.citas));
    

    fstream archivo(nombreArchivo, std::ios::binary | std::ios::app);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de doctores para agregar." << std::endl;
        return false;
    }
    
    
    archivo.write((char*)&nuevoDoctor, sizeof(Doctor));
    archivo.close();
    
    
    ArchivoHeader header = leerHeader(nombreArchivo);
    header.cantidadRegistros++;
    header.registrosActivos++;
    actualizarHeader(nombreArchivo, header);
    
    
    h->siguienteIdDoctor++;
    h->totalDoctoresRegistrados++;
    guardarDatosHospital(h); 
    
    cout << "Doctor ID " << nuevoDoctor.id << " registrado exitosamente.\n";
    return true;
}




int buscarIndiceDoctorDeID(int id) {
    const char* nombreArchivo = "doctores.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) return -1;
    
    // Leer el Header
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    Doctor temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Doctor));
        
        
        if (temp.id == id && !temp.eliminado) {
            archivo.close();
            return i; 
        }
    }
    
    archivo.close();
    return -1; 
}
Doctor obtenerDoctorPorIndice(int indice) {
    Doctor d = {0}; 
    
    if (indice < 0) return d;

    ifstream archivo("doctores.bin", std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir doctores.bin para lectura aleatoria." << std::endl;
        return d;
    }

    
    long posicion = calcularPosicion(indice, sizeof(Doctor));
    
    
    archivo.seekg(posicion, std::ios::beg);
    archivo.read((char*)&d, sizeof(Doctor));
    
    archivo.close();
    return d;
}
bool actualizarDoctor(Doctor doctorModificado);

Doctor obtenerDoctorPorID(int id) {
    // 1. Encontrar la ubicación del registro
    int indice = buscarIndiceDoctorDeID(id);
    
    // 2. Si se encuentra, leer el registro directamente (Acceso Aleatorio)
    if (indice != -1) {
        return obtenerDoctorPorIndice(indice);
    }
    
    // 3. Si no se encuentra, devolver una estructura Doctor vacía (ID 0)
    return Doctor{0}; 
}

Doctor obtenerDoctorPorCedula(const char* cedula) {
    const char* nombreArchivo = "doctores.bin";
    Doctor dEncontrado = {0}; 
    
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) return dEncontrado;
    
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    Doctor temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // Comprobar cédula y estado 'eliminado'
        if (!temp.eliminado && strcmp(temp.cedula, cedula) == 0) { 
            archivo.close();
            return temp; // ¡Cédula encontrada!
        }
    }
    
    archivo.close();
    return dEncontrado; 
}

Doctor obtenerDoctorPorEspecialidad(const char* especialidad) {
    const char* nombreArchivo = "doctores.bin";
    Doctor dEncontrado = {0}; 
    
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) return dEncontrado;
    
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    Doctor temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // Comprobar especialidad y estado 'eliminado'
        // Se usa strcmp para una coincidencia exacta de la especialidad.
        if (!temp.eliminado && strcmp(temp.especialidad, especialidad) == 0) { 
            archivo.close();
            return temp; // ¡Especialidad encontrada!
        }
    }
    
    archivo.close();
    return dEncontrado; 
}
bool asignarPacienteADoctor_Disco(int idDoctor, int idPaciente) {

    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) {
        std::cerr << "Error: Doctor ID " << idDoctor << " no encontrado.\n";
        return false;
    }
    
    
    const int CAPACIDAD_MAXIMA = 50; 

    
    if (doctor.capacidadPacientes >= CAPACIDAD_MAXIMA) {
        cerr << "Advertencia: El Dr. " << doctor.nombre << " ha alcanzado el límite de pacientes (" << CAPACIDAD_MAXIMA << ").\n";
        return false;
    }

    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            cerr << "Advertencia: Paciente ID " << idPaciente << " ya estaba asignado a este doctor.\n";
            return false;
        }
    }

    // 3. Añadir el ID del Paciente al array fijo
    doctor.pacientesIDs[doctor.capacidadPacientes++] = idPaciente;
    doctor.fechaModificacion = time(NULL); // Actualizar metadata

    // 4. Sobrescribir el registro del Doctor en doctores.bin (Escritura con Acceso Aleatorio)
    if (actualizarDoctor(doctor)) {
        cout << "Paciente ID " << idPaciente << " asignado al Dr. " << doctor.nombre << ".\n";
        return true;
    }
    
    cerr << "Error al actualizar el registro del doctor en disco.\n";
    return false;
}

void listarPacientesDeDoctor_Disco(int idDoctor) {
    // 1. Obtener el Doctor (lectura desde disco)
    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) {
        cerr << "Error: Doctor ID " << idDoctor << " no encontrado.\n";
        return;
    }

    cout << "\nPacientes asignados al Dr. " << doctor.nombre << " (" << doctor.especialidad << "): " << std::endl;
    cout << "Total de pacientes: " << doctor.capacidadPacientes << "\n";
    
    if (doctor.capacidadPacientes == 0) {
        cout << "Este doctor no tiene pacientes asignados.\n";
        return;
    }

    // 2. Iterar sobre el array fijo de IDs
    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        int idPaciente = doctor.pacientesIDs[i];
        
        // 3. Obtener el Paciente (Acceso Aleatorio a pacientes.bin)
        Paciente p = obtenerPacientePorID(idPaciente); 
        
        
        if (p.id != 0 && !p.eliminado) {
           
            cout << "- " << p.nombre << " (ID: " << p.id << ", Cédula: " << p.cedula << ")\n";
        } else {
            
            cout << "- [ID: " << idPaciente << "] Paciente no encontrado o eliminado.\n";
        }
    }
}
bool removerPacienteDeDoctor_Disco(int idDoctor, int idPaciente) {
    
    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) return false;

    int index = -1;
    
    
    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            index = i;
            break;
        }
    }
    if (index == -1) return false; 


    for (int i = index; i < doctor.capacidadPacientes - 1; i++) {
        doctor.pacientesIDs[i] = doctor.pacientesIDs[i + 1];
    }
    doctor.capacidadPacientes--;
    doctor.pacientesIDs[doctor.capacidadPacientes] = 0; 
    doctor.fechaModificacion = time(NULL);

    
    if (actualizarDoctor(doctor)) {
        cout << "Paciente ID " << idPaciente << " removido del Dr. " << doctor.nombre << ".\n";
        return true;
    }
    
    cerr << "Error al actualizar el registro del doctor en disco tras la remoción.\n";
    return false;
}
void listarTodosDoctores() {
    const char* nombreArchivo = "doctores.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de doctores para listado." <<endl;
        return;
    }
    
    // 1. Leer el Header para obtener la cantidad total de registros.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 
    
    // Configuración de la tabla
    cout << "\n+-------------------------------------------------------------------+\n";
    cout << "| " << setw(65) << left << "LISTA DE DOCTORES ACTIVOS" << " |\n";
    cout << "+-----+------------------------+----------------+---------------------+\n";
    cout << "| ID  | NOMBRE COMPLETO        | CEDULA         | ESPECIALIDAD        |\n";
    cout << "+-----+------------------------+----------------+---------------------+\n";

    Doctor temp; // Variable temporal para contener UN solo registro a la vez
    int activosEncontrados = 0;
    
    // 2. Iterar sobre la cantidad TOTAL de registros en el archivo.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Leer el siguiente registro del archivo de manera secuencial.
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // 3. Comprobar que el registro no esté eliminado.
        if (!temp.eliminado) {
            // 4. Mostrar el registro activo con formato.
            cout << "| " << setw(3) << temp.id << " | "
                      << setw(22) << left << temp.nombre << " | "
                      << setw(14) << temp.cedula << " | "
                      << setw(19) << temp.especialidad << " |\n";
            activosEncontrados++;
        }
        
        if (archivo.fail()) break;
    }
    
    cout << "+-----+------------------------+----------------+---------------------+\n";
    cout << "Total de doctores listados: " << activosEncontrados 
              << " (Activos según Header: " << header.registrosActivos << ")" << std::endl;
              
    archivo.close();
}
int compararCadenas(const char* s1, const char* s2) {
    // Itera mientras ambas cadenas tengan caracteres
    while (*s1 && *s2) {
        // Compara los caracteres convertidos a minúsculas
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 1; // Diferentes
        }
        s1++;
        s2++;
    }
    
    // Si ambas terminaron al mismo tiempo, son iguales.
    // Se usa el operador ternario para retornar 0 (iguales) o 1 (diferentes)
    return (*s1 == *s2) ? 0 : 1; 
}
void buscarDoctoresPorEspecialidad(const char* especialidadBuscada) {
    const char* nombreArchivo = "doctores.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de doctores." << endl;
        return;
    }
    
    ArchivoHeader header;
    // 1. Leer el encabezado para saber cuántos registros hay
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    
    Doctor temp;
    int encontrados = 0;
    
    cout << "\n--- Doctores en la especialidad: " << especialidadBuscada << " ---\n";

    // 2. Iterar sobre todos los registros escritos
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // 3. Leer un registro
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // 
        if (!temp.eliminado) { 
            
            if (compararCadenas(temp.especialidad, especialidadBuscada) == 0) {
                
                cout << "ID: " << temp.id 
                          << ", Nombre: " << temp.nombre 
                          << ", Cédula: " << temp.cedula 
                          << ", Pacientes asignados: " << temp.capacidadPacientes
                          << endl;
                encontrados++;
            }
        }
    }
    
    archivo.close();

    if (encontrados == 0) {
        cout << "No se encontraron doctores activos en esa especialidad." << endl;
    }
}

bool eliminarDoctor(Hospital* h, int id) {
    const char* nombreArchivo = "doctores.bin";
    
    // 1. Obtener el Doctor y verificar existencia.
    Doctor d = obtenerDoctorPorID(id);
    
    // Si el ID es 0 o ya está marcado como eliminado, no existe o es redundante.
    if (d.id == 0 || d.eliminado) {
        cerr << "Error: Doctor ID " << id << " no encontrado o ya estaba eliminado.\n";
        return false;
    }

    // 2. Aplicar el borrado LÓGICO (cambiar la bandera).
    d.eliminado = true;
    d.fechaModificacion = time(NULL);

    if (!actualizarDoctor(d)) {
        cerr << "Error: No se pudo sobrescribir el registro marcado como eliminado.\n";
        return false;
    }

    ArchivoHeader header = leerHeader(nombreArchivo);
    header.registrosActivos--; // Disminuir el contador de activos
    actualizarHeader(nombreArchivo, header);
    
    cout << "Doctor ID " << id << " eliminado logicamente. Registros activos actualizados.\n";
    return true;
}

//Gestion de citas
bool verificarConflictoCita(int idDoctor, const char* fecha, const char* hora) {
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Cita temp;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // El conflicto solo aplica a citas activas, pendientes y del mismo doctor/fecha/hora
        if (!temp.eliminado && temp.idDoctor == idDoctor && 
            strcmp(temp.fecha, fecha) == 0 && strcmp(temp.hora, hora) == 0 &&
            !temp.atendida) {
            archivo.close();
            return true; // Conflicto encontrado
        }
    }
    archivo.close();
    return false; // No hay conflicto
}


bool agregarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, const char* hora) {
    const char* nombreArchivo = "citas.bin";

    // 1. Verificar conflicto de horario
    if (verificarConflictoCita(idDoctor, fecha, hora)) {
        cout << "Ya existe una cita con el doctor en esa fecha y hora.\n";
        return false;
    }

    
    Cita nuevaCita;
    nuevaCita.id = h->siguienteIdCita;
    nuevaCita.id = idPaciente;
    nuevaCita.id = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha));
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora));
    strcpy(nuevaCita.motivo, "Cita agendada por usuario"); 
    strcpy(nuevaCita.estado, "pendiente");
    nuevaCita.atendida = false;
    nuevaCita.eliminado = false; 
    nuevaCita.id = -1; // Sin historial asociado inicialmente

    // 3. Guardar la Cita en citas.bin (CREATE)
    fstream archivo(nombreArchivo, ios::binary | ios::app);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para agregar cita." << endl;
        return false;
    }
    archivo.write((char*)&nuevaCita, sizeof(Cita));
    archivo.close();

    
    Paciente p = obtenerPacientePorID(idPaciente); 
    Doctor d = obtenerDoctorPorID(idDoctor); 
    
    // (Lógica de añadir el ID de la cita al array fijo p.citasIDs y d.citasIDs)
    if (p.id != 0 && p.cantidadCitas < 20) {
       p.citasIDs[p.cantidadCitas] = nuevaCita.id;
       p.cantidadCitas++;
        actualizarPaciente(p); 
    }
    if (d.id != 0 && d.cantidadCitas < 30) {
        d.citasIDs [d.cantidadCitas++] = nuevaCita.id;
        actualizarDoctor(d); 
    }

    // 5. Actualizar Contadores
    ArchivoHeader header = leerHeader(nombreArchivo);
    header.cantidadRegistros++;
    header.registrosActivos++;
    actualizarHeader(nombreArchivo, header);
    
    h->siguienteIdCita++;
    h->totalCitasAgendadas++;
    guardarDatosHospital(h); 
    
    cout << "Cita agendada con ID: " << nuevaCita.id << "\n";
    return true;
}
Cita obtenerCitaPorIndice(int indiceBuscado);

int buscarIndiceCitaDeID(int idBuscado);

bool atenderCita(Hospital* h, int idCita, const char* diagnostico, double costo);

bool actualizarCita(Cita citaModificada);

bool cancelarCita_Disco(int idCita) {
    // 1. Buscar la cita y obtener su índice en disco
    int indice = buscarIndiceCitaDeID(idCita); 
    if (indice == -1) {
        cerr << "Error: Cita ID " << idCita << " no encontrada para cancelar.\n";
        return false;
    }

    // 2. Leer la cita de disco
    Cita cita = obtenerCitaPorIndice(indice);
    
    // 3. Modificar el estado (borrado lógico de estado)
    if (strcmp(cita.estado, "pendiente") != 0) {
        cerr << "Error: Cita ID " << idCita << " no está pendiente y no puede ser cancelada.\n";
        return false;
    }
    
    strcpy(cita.estado, "cancelada");
    cita.fechaModificacion = time(NULL);

    // 4. Sobrescribir el registro actualizado en citas.bin
    if (actualizarCita(cita)) {
        cout << "Cita ID " << idCita << " cancelada exitosamente.\n";
        
        return true;
    }

    cerr << "Error al actualizar el registro de la cita en disco.\n";
    return false;
}



// Asumimos que las estructuras Cita y ArchivoHeader están definidas.

void listarCitasPorPaciente(int idPaciente) {
    const char* nombreArchivo = "citas.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir citas.bin para listado.\n";
        return;
    }
    
    // Leer el encabezado para saber cuántas citas leer.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    cout << "\n+-------+----------+--------+---------+---------------------+\n";
    cout << "| " << setw(52) << left << "CITAS DEL PACIENTE ID " << idPaciente << " |\n";
    cout << "+-------+----------+--------+---------+---------------------+\n";
    cout << "| ID    | Fecha    | Hora   | Dr. ID  | Estado              |\n";
    cout << "+-------+----------+--------+---------+---------------------+\n";

    Cita temp;
    bool encontrado = false;
    
    // Iteración secuencial sobre el archivo
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // Comprobar borrado lógico y el ID del paciente
        if (!temp.eliminado && temp.idPaciente == idPaciente) {
            cout << "| " << setw(5) << temp.id 
                      << " | " << setw(8) << temp.fecha 
                      << " | " << setw(6) << temp.hora 
                      << " | " << setw(7) << temp.idDoctor  
                      << " | " << setw(19) << left << temp.estado << " |\n";
            encontrado = true;
        }
        if (archivo.fail()) break;
    }
    
    cout << "+-------+----------+--------+---------+---------------------+\n";
    if (!encontrado) {
         cout << "| " << setw(52) << left << "No se encontraron citas activas para este paciente." << " |\n";
         cout << "+-------+----------+--------+---------+---------------------+\n";
    }
    archivo.close();
}

void listarCitasPorDoctor(int idDoctor) {
    const char* nombreArchivo = "citas.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir citas.bin para listado.\n";
        return;
    }
    
    // Leer el encabezado para saber cuántas citas leer.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    cout << "\n+-------+----------+--------+---------+---------------------+\n";
    cout << "| " << setw(52) << left << "CITAS DEL DOCTOR ID " << idDoctor << " |\n";
    cout << "+-------+----------+--------+---------+---------------------+\n";
    cout << "| ID    | Fecha    | Hora   | Pcte. ID| Estado              |\n";
    cout << "+-------+----------+--------+---------+---------------------+\n";

    Cita temp;
    bool encontrado = false;
    
    // Iteración secuencial sobre el archivo
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // Comprobar borrado lógico y el ID del doctor
        if (!temp.eliminado && temp.idDoctor == idDoctor) {
            cout << "| " << setw(5) << temp.id 
                      << " | " <<setw(8) << temp.fecha 
                      << " | " << setw(6) << temp.hora 
                      << " | " << setw(7) << temp.idPaciente 
                      << " | " << setw(19) << left << temp.estado << " |\n";
            encontrado = true;
        }
        if (archivo.fail()) break;
    }
    
    cout << "+-------+----------+--------+---------+---------------------+\n";
    if (!encontrado) {
         std::cout << "| " << std::setw(52) << std::left << "No se encontraron citas activas para este doctor." << " |\n";
         cout << "+-------+----------+--------+---------+---------------------+\n";
    }
    archivo.close();
}


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

void destruirHospital(Hospital* hospital) {
    cout << "\nGuardando configuracion global del hospital...\n";
    
    if (guardarDatosHospital(hospital)) {
        cout << "Estado global guardado exitosamente.\n";
    } else {
        cerr << "ADVERTENCIA: Fallo al guardar el estado global del hospital. Los IDs podrian resetearse.\n";
    }

    cout << "Destruccion de memoria en RAM completada.\n";
}

int obtenerEntero(const char* prompt) {
    int valor;
    cout << prompt;
    while (!(cin >> valor)) {
        cerr << "Entrada invalida. Ingrese un numero entero: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return valor;
}
bool agregarPaciente(Hospital* h); 

int main() {
    Hospital hospital;
    
    // 1. CARGA DE ESTADO GLOBAL (REEMPLAZA TODA LA INICIALIZACIÓN DE ARRAYS)
    if (!cargarDatosHospital(&hospital)) {
        cerr << "ADVERTENCIA: No se pudo cargar el estado del hospital. Inicializando IDs a 1.\n";
        // Si la carga falla, aseguramos que los IDs comiencen en 1.
        hospital.siguienteIdPaciente = 1;
        hospital.siguienteIdDoctor = 1;
        hospital.siguienteIdCita = 1;
        hospital.totalPacientesRegistrados = 0; // Se asume que estos campos se usan para contadores
        hospital.totalDoctoresRegistrados= 0;
        hospital.totalCitasAgendadas = 0;
    }

    int opcion;
    do {
        // Mostrar el menú
        cout << "\n+--------------------------------------+\n";
        cout << "|      MENU PRINCIPAL DEL HOSPITAL     |\n";
        cout << "+--------------------------------------+\n";
        cout << "| 1. Registrar paciente                |\n";
        cout << "| 2. Registrar doctor                  |\n";
        cout << "| 3. Agendar cita                      |\n";
        cout << "| 4. Atender cita                      |\n";
        cout << "| 5. Mostrar historial de paciente     |\n";
        cout << "| 6. Cancelar Cita                     |\n"; 
        cout << "| 7. Listar doctores                   |\n"; 
        cout << "| 8. Listar pacientes                  |\n"; 
        cout << "| 9. Buscar paciente por Cedula        |\n"; 
        cout << "| 10. Buscar doctor por Especialidad   |\n"; 
        cout << "| 11. Buscar pacientes por Nombre      |\n"; 
        cout << "| 12. Eliminar paciente (Lógico)       |\n"; 
        cout << "| 13. Eliminar doctor (Lógico)         |\n"; 
        cout << "| 0. Salir y Guardar                   |\n";
        cout << "+--------------------------------------+\n";
        opcion = obtenerEntero("Seleccione una opcion: ");

        

        switch (opcion) {
            case 1: { 
                agregarPaciente(&hospital); 
                break;
            }
            case 2: { 
                agregarDoctor(&hospital); 
                break;
            }
            case 3: { 
                int idPaciente = obtenerEntero("ID del paciente: ");
                int idDoctor = obtenerEntero("ID del doctor: ");
                char fecha[11], hora[6];
                cout << "Fecha (DD/MM/AAAA): "; cin.getline(fecha, 11);
                cout << "Hora (HH:MM): "; cin.getline(hora, 6);
                agregarCita(&hospital, idPaciente, idDoctor, fecha, hora);
                break;
            }
            case 4: { 
                int idCita = obtenerEntero("ID de la cita a atender: ");
                char diagnostico[100];
                float costo;
                
                cout << "Diagnostico: "; cin.getline(diagnostico, 100);
                
                cout << "Costo de la consulta: ";
                if (!(cin >> costo)) { costo = 0.0f; cin.clear(); }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                atenderCita(&hospital, idCita, diagnostico, costo);
                break;
            }
            case 5: { 
                int idPaciente = obtenerEntero("Ingrese el ID del paciente para ver historial: ");
                mostrarHistorial(idPaciente);
                break;
            }
            case 6: { 
                int idCancelar = obtenerEntero("Ingrese el ID de la cita que desea cancelar: ");
                cancelarCita_Disco(idCancelar); 
                break;
            }
            case 7: 
                listarTodosDoctores(); 
                break;
            case 8: 
                listarTodosPacientes(); 
                break;
            case 9: { 
                char cedula[21];
                cout << "Ingrese la cedula del paciente: ";
                cin.getline(cedula, 21);
                Paciente p = obtenerPacientePorCedula(cedula); 
                if (!p.eliminado && p.id != 0) {
                    cout << "Paciente encontrado: " << p.nombre << " " << p.apellido << ", Edad: " << p.edad << "\n";
                } else {
                    cout << "Paciente no encontrado.\n";
                }
                break;
            }
            case 10: { 
                char especialidad[51];
                cout << "Ingrese la especialidad a buscar: ";
                cin.getline(especialidad, 51);
                buscarDoctoresPorEspecialidad(especialidad); 
                break;
            }
            case 11: { 
                char nombreParcial[51];
                cout << "Ingrese parte del nombre del paciente: ";
                cin.getline(nombreParcial, 51);
                buscarPacientesPorNombreParcial(nombreParcial); 
                break;
            }
            case 12: { 
                int idPaciente = obtenerEntero("ID del paciente a ELIMINAR logicamente: ");
                eliminarPaciente(&hospital, idPaciente); 
                break;
            }
            case 13: { 
                int idDoctor = obtenerEntero("ID del doctor a ELIMINAR logicamente: ");
                eliminarDoctor(&hospital, idDoctor); 
                break;
            }
            case 0:
                cout << "Gracias por usar el sistema del hospital. Guardando estado...\n";
                break;
            default:
                cout << "Opción no válida. Intente de nuevo.\n";
                break;
        }
        
        
        if (opcion != 0) {
            cout << "\nPresione ENTER para continuar...\n";
            cin.get();
            system("cls || clear");
        }

    } while (opcion != 0);

    
    destruirHospital(&hospital); 

    return 0;
}

