#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;   




//estructura de hospital la doctoresa 

// Estructura HistorialMedico
struct HistorialMedico {
    int id;
    char fecha[11];
    char diagnostico[500];
    char tratamiento[500];
    char observaciones[500];
    int idPaciente;

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

    char observaciones[200];
    int ConsultaID;
    bool confirmada;
    bool atendida;
    int consultasID;

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
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para lectura del Header: " << nombreArchivo << std::endl;
        return header;
    }
    
    // Leer exactamente el tamaño del Header desde el inicio (posición 0)
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    
    archivo.close();
    return header;
}
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para actualizar el Header: " << nombreArchivo << std::endl;
        return false;
    }
    
    // Posicionarse al inicio del archivo (byte 0) para la escritura
    archivo.seekp(0, std::ios::beg);
    
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
    
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::out);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo crear o abrir el archivo para inicializar: " 
                  << nombreArchivo << std::endl;
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
    // Asegúrate de inicializar los arrays fijos (citasIDs) a 0 o -1 aquí si es necesario.

    // 2. Abrir el archivo en modo APPEND (ios::app) para escribir al final.
    // Ojo: Si usas ios::app, el puntero se posiciona al final, IGNORANDO el Header.
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para agregar paciente." << std::endl;
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
    
    
    std::cout << "Paciente ID " << nuevoPaciente.id << " registrado exitosamente." << std::endl;
    return true;
}

int buscarIndiceDeID(int id) {
    const char* nombreArchivo = "pacientes.bin";
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de pacientes para busqueda." << std::endl;
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

Paciente leerPacientePorIndice(int indice) {
    Paciente p;
    
    if (indice < 0) return p; // Manejo de error si el índice es inválido

    std::ifstream archivo("pacientes.bin", std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir pacientes.bin para lectura aleatoria." << std::endl;
        return p;
    }

    // Usar la función de bajo nivel que ya implementaste
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // Posicionarse directamente en el inicio del registro
    archivo.seekg(posicion, std::ios::beg);
    
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
        std::cerr << "Error: Paciente con ID " << pModificado.id << " no encontrado para actualizar." << std::endl;
        return false;
    }

    // 2. Calcular la posición exacta en bytes.
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // 3. Abrir el archivo en modo de lectura y escritura binaria.
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para actualizar paciente." << std::endl;
        return false;
    }
    
    // 4. Actualizar la metadata de modificación.
    pModificado.fechaModificacion = time(NULL);

    // 5. Posicionar el puntero de escritura directamente en el inicio del registro.
    archivo.seekp(posicion, std::ios::beg);
    
    // 6. Escribir/Sobrescribir el registro completo.
    archivo.write((char*)&pModificado, sizeof(Paciente));
    
    archivo.close();
    std::cout << "Paciente ID " << pModificado.id << " actualizado exitosamente." << std::endl;
    return true;
}
bool eliminarPaciente(Hospital* h, int id) {
    const char* nombreArchivo = "pacientes.bin";
    
    // 1. Obtener la copia del registro actual.
    Paciente p = obtenerPacientePorID(id);
    
    // El ID 0 se utiliza para indicar un registro vacío/no encontrado.
    if (p.id == 0 || p.eliminado) {
        std::cerr << "Error: Paciente ID " << id << " no encontrado o ya estaba eliminado." << std::endl;
        return false;
    }

    // 2. Aplicar el borrado LÓGICO (la bandera).
    p.eliminado = true;
    p.fechaModificacion = time(NULL);

    // 3. Sobrescribir el registro en el archivo (Usando la función actualizarPaciente).
    if (!actualizarPaciente(p)) {
        std::cerr << "Error: No se pudo sobrescribir el registro marcado como eliminado." << std::endl;
        return false;
    }
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
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de pacientes para listado." << std::endl;
        return;
    }
    
    // 1. Leer el Header para obtener la cantidad total de registros.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 
    
    // Configuración de la tabla
    std::cout << "\n+---------------------------------------------------------------+\n";
    std::cout << "| " << std::setw(59) << std::left << "LISTA DE PACIENTES ACTIVOS" << " |\n";
    std::cout << "+-----+----------------------+---------------+------+-----------+\n";
    std::cout << "| ID  | NOMBRE COMPLETO      | CEDULA        | EDAD | CONSULTAS |\n";
    std::cout << "+-----+----------------------+---------------+------+-----------+\n";

    Paciente temp; // Variable temporal para contener UN solo registro a la vez
    int activosEncontrados = 0;
    
    // 2. Iterar sobre la cantidad TOTAL de registros en el archivo.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Leer el siguiente registro del archivo de manera secuencial.
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // 3. Comprobar que el registro no esté eliminado.
        if (!temp.eliminado) {
            // 4. Mostrar el registro activo con formato.
            std::cout << "| " << std::setw(3) << temp.id << " | "
                      << std::setw(20) << std::left << temp.nombre << " | "
                      << std::setw(13) << temp.cedula << " | "
                      << std::setw(4) << temp.edad << " | "
                      // Usamos cantidadConsultas, ya que capacidadHistorial fue eliminada.
                      << std::setw(9) << temp.cantidadConsultas << " |\n";
            activosEncontrados++;
        }
        
        if (archivo.fail()) break;
    }
    
    std::cout << "+-----+----------------------+---------------+------+-----------+\n";
    std::cout << "Total de pacientes listados: " << activosEncontrados 
              << " (Activos según Header: " << header.registrosActivos << ")" << std::endl;
              
    archivo.close();
}

void buscarPacientesPorNombreParcial(const char* nombreParcial) {
    const char* nombreArchivo = "pacientes.bin";
    bool encontrado = false;
    
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de pacientes para busqueda parcial." << std::endl;
        return;
    }
    
    // Leer el Header para iterar correctamente
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    // Configuración de la tabla
    std::cout << "+---------------------------------------------------------------+\n";
    std::cout << "| " << std::setw(59) << std::left << "PACIENTES QUE COINCIDEN CON EL NOMBRE" << " |\n";
    std::cout << "+-----+----------------------+---------------+------+-----------+\n";
    std::cout << "| ID  | NOMBRE COMPLETO      | CÉDULA        | EDAD | CONSULTAS |\n";
    std::cout << "+-----+----------------------+---------------+------+-----------+\n";

    Paciente temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Paciente));
        
        // Comprobación: 1. No eliminado, 2. Nombre parcial coincide
        // Usamos strstr para buscar una subcadena.
        if (!temp.eliminado && strstr(temp.nombre, nombreParcial) != nullptr) { 
            std::cout << "| " << std::setw(3) << temp.id << " | "
                      << std::setw(20) << std::left << temp.nombre << " | "
                      << std::setw(13) << temp.cedula << " | "
                      << std::setw(4) << temp.edad << " | "
                      << std::setw(9) << temp.cantidadConsultas << " |\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        std::cout << "| " << std::setw(59) << std::left << "No se encontraron pacientes activos con ese nombre parcial." << " |\n";
    }

    std::cout << "+-----+----------------------+---------------+------+-----------+\n";
    archivo.close();
}

//historial medico del pasciente 
int agregarHistorialMedico(Hospital* h, HistorialMedico nuevaConsulta);

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

        // 'idAnterior' es el ID de la ÚLTIMA consulta existente.
        consultaActual.siguienteConsultaID = nuevaConsultaID; // Lo enlazamos al nuevo registro
        
        
        actualizarHistorialMedico(consultaActual); 
    }
    pacienteActual.cantidadConsultas++;
    actualizarPaciente(pacienteActual); // Sobrescribe el paciente con el nuevo enlace/contador.

    return true;
}
void agregarHistorial(Hospital* h, int pacienteID) {
    // Ya no necesitas checar capacidad; la capacidad es el disco.

    HistorialMedico nuevo;
    
    // El ID de la consulta lo asigna la función enlazarYAgregarConsulta, 
    // pero puedes seguir pidiendo otros datos.

    std::cout << "Fecha (dd/mm/aaaa): ";
    std::cin.getline(nuevo.fecha, 20);

    std::cout << "Hora (hh:mm): ";
    std::cin.getline(nuevo.hora, 10);

    std::cout << "Diagnóstico: ";
    std::cin.getline(nuevo.diagnostico, 100);
    
    std::cout << "Tratamiento: ";
    std::cin.getline(nuevo.tratamiento, 500); // Asumiendo que agregaste este campo

    std::cout << "ID del doctor: ";
    std::cin >> nuevo.id;
    std::cin.ignore();

    std::cout << "Costo: ";
    std::cin >> nuevo.costo;
    std::cin.ignore();

    // Llamar a la función de enlace para guardarlo en el archivo historiales.bin
    if (enlazarYAgregarConsulta(h, pacienteID, nuevo)) {
        std::cout << "Historial agregado correctamente.\n";
    } else {
        std::cerr << "ERROR: No se pudo agregar el historial o el paciente no existe.\n";
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

    std::cout << "\nHISTORIAL MEDICO DEL PACIENTE ID: " << pacienteID << "\n";
    std::cout << "+---------------------------------------------------------------------------------------------+\n";
    std::cout << "| ID |    Fecha    |  Hora  |      Diagnostico      | ID Doctor |   Costo  |\n";
    std::cout << "+---------------------------------------------------------------------------------------------+\n";

    int idActual = paciente.primerConsultaID;
    
    // 2. Recorrer la lista en disco
    while (idActual != -1) {
        // Asumimos que existe la función obtenerHistorialMedicoPorID(int idConsulta);
        HistorialMedico h = obtenerHistorialMedicoPorID(idActual);
        
        // Mostrar datos
        std::cout << "| " << std::setw(2) << h.id 
                  << " | " << std::setw(11) << h.fecha
                  << " | " << std::setw(6) << h.hora
                  << " | " << std::setw(21) << std::left << h.diagnostico
                  << " | " << std::setw(9) << h.id
                  << " | $" << std::setw(7) << std::right << std::fixed << std::setprecision(2) << h.costo << " |\n";
        
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
bool agregarDoctor(Hospital* h) {
    const char* nombreArchivo = "doctores.bin";
    Doctor nuevoDoctor;
    
    std::cout << "\n--- REGISTRO DE NUEVO DOCTOR ---\n";
    
    // 1. Recolección de Datos del Usuario
    std::cout << "Nombre completo: ";
    std::cin.getline(nuevoDoctor.nombre, sizeof(nuevoDoctor.nombre));

    std::cout << "Cédula: ";
    std::cin.getline(nuevoDoctor.cedula, sizeof(nuevoDoctor.cedula));
    
    std::cout << "Edad: ";
    if (!(std::cin >> nuevoDoctor.edad)) return false;
    std::cin.ignore();

    std::cout << "Teléfono: ";
    std::cin.getline(nuevoDoctor.telefono, sizeof(nuevoDoctor.telefono));
    
    std::cout << "Especialidad: ";
    std::cin.getline(nuevoDoctor.especialidad, sizeof(nuevoDoctor.especialidad));
    
    // 2. Asignar ID y Metadata
    nuevoDoctor.id = h->siguienteIdDoctor;
    nuevoDoctor.eliminado = false; // El registro está activo
    nuevoDoctor.fechaCreacion = time(NULL);
    nuevoDoctor.fechaModificacion = time(NULL);
    
    // Inicializar contadores de relaciones
    nuevoDoctor.capacidadPacientes = 0;
    nuevoDoctor.cantidadCitas = 0;

    // Inicializar los arrays de IDs fijos a 0 o -1 (por si acaso)
    memset(nuevoDoctor.pacientesIDs, 0, sizeof(nuevoDoctor.pacientesIDs));
    memset(nuevoDoctor.citasIDs, 0, sizeof(nuevoDoctor.citasIDs));
    
    // 3. Abrir el archivo en modo APPEND (CREATE)
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de doctores para agregar." << std::endl;
        return false;
    }
    
    // 4. Escribir el nuevo registro de Doctor.
    archivo.write((char*)&nuevoDoctor, sizeof(Doctor));
    archivo.close();
    
    // 5. Actualizar el Header del archivo (doctores.bin).
    ArchivoHeader header = leerHeader(nombreArchivo);
    header.cantidadRegistros++;
    header.registrosActivos++;
    actualizarHeader(nombreArchivo, header);
    
    // 6. Actualizar los contadores globales del Hospital (en RAM y en disco).
    h->siguienteIdDoctor++;
    h->totalDoctoresRegistrados++;
    guardarDatosHospital(h); // Guardar los contadores actualizados en hospital.bin
    
    std::cout << "Doctor ID " << nuevoDoctor.id << " registrado exitosamente.\n";
    return true;
}




int buscarIndiceDoctorDeID(int id) {
    const char* nombreArchivo = "doctores.bin";
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) return -1;
    
    // Leer el Header
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    Doctor temp; 
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // Comprobar ID y estado 'eliminado'
        if (temp.id == id && !temp.eliminado) {
            archivo.close();
            return i; // ¡Índice encontrado!
        }
    }
    
    archivo.close();
    return -1; // ID no encontrado.
}
Doctor obtenerDoctorPorIndice(int indice) {
    Doctor d = {0}; // Inicializar struct vacío
    
    if (indice < 0) return d;

    std::ifstream archivo("doctores.bin", std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir doctores.bin para lectura aleatoria." << std::endl;
        return d;
    }

    // Usar la función de bajo nivel para calcular la posición en bytes
    long posicion = calcularPosicion(indice, sizeof(Doctor));
    
    // Posicionarse y leer
    archivo.seekg(posicion, std::ios::beg);
    archivo.read((char*)&d, sizeof(Doctor));
    
    archivo.close();
    return d;
}
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
    
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
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
    // 1. Obtener la copia del registro del Doctor (lectura desde disco)
    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) {
        std::cerr << "Error: Doctor ID " << idDoctor << " no encontrado.\n";
        return false;
    }
    
    // Asumimos que la capacidad máxima del array es 50
    const int CAPACIDAD_MAXIMA = 50; 

    // 2. Verificar si el paciente ya está asignado y si hay capacidad
    if (doctor.capacidadPacientes >= CAPACIDAD_MAXIMA) {
        std::cerr << "Advertencia: El Dr. " << doctor.nombre << " ha alcanzado el límite de pacientes (" << CAPACIDAD_MAXIMA << ").\n";
        return false;
    }

    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            std::cerr << "Advertencia: Paciente ID " << idPaciente << " ya estaba asignado a este doctor.\n";
            return false;
        }
    }

    // 3. Añadir el ID del Paciente al array fijo
    doctor.pacientesIDs[doctor.capacidadPacientes++] = idPaciente;
    doctor.fechaModificacion = time(NULL); // Actualizar metadata

    // 4. Sobrescribir el registro del Doctor en doctores.bin (Escritura con Acceso Aleatorio)
    if (actualizarDoctor(doctor)) {
        std::cout << "Paciente ID " << idPaciente << " asignado al Dr. " << doctor.nombre << ".\n";
        return true;
    }
    
    std::cerr << "Error al actualizar el registro del doctor en disco.\n";
    return false;
}

void listarPacientesDeDoctor_Disco(int idDoctor) {
    // 1. Obtener el Doctor (lectura desde disco)
    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) {
        std::cerr << "Error: Doctor ID " << idDoctor << " no encontrado.\n";
        return;
    }

    std::cout << "\nPacientes asignados al Dr. " << doctor.nombre << " (" << doctor.especialidad << "): " << std::endl;
    std::cout << "Total de pacientes: " << doctor.capacidadPacientes << "\n";
    
    if (doctor.capacidadPacientes == 0) {
        std::cout << "Este doctor no tiene pacientes asignados.\n";
        return;
    }

    // 2. Iterar sobre el array fijo de IDs
    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        int idPaciente = doctor.pacientesIDs[i];
        
        // 3. Obtener el Paciente (Acceso Aleatorio a pacientes.bin)
        Paciente p = obtenerPacientePorID(idPaciente); 
        
        // 4. Mostrar información si el paciente existe y no ha sido eliminado
        if (p.id != 0 && !p.eliminado) {
            // Nota: Asumimos que el nombre ya incluye el apellido, 
            // ya que la estructura Paciente solo tiene 'nombre'
            std::cout << "- " << p.nombre << " (ID: " << p.id << ", Cédula: " << p.cedula << ")\n";
        } else {
            // Este caso maneja pacientes que fueron eliminados lógicamente pero su ID sigue en el Doctor.
            std::cout << "- [ID: " << idPaciente << "] Paciente no encontrado o eliminado.\n";
        }
    }
}
bool removerPacienteDeDoctor_Disco(int idDoctor, int idPaciente) {
    // 1. Obtener la copia del registro del Doctor
    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) return false;

    int index = -1;
    
    // 2. Buscar el índice del ID del Paciente en el array fijo
    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            index = i;
            break;
        }
    }
    if (index == -1) return false; // El paciente no estaba asignado

    // 3. Desplazar los elementos para remover el ID
    for (int i = index; i < doctor.capacidadPacientes - 1; i++) {
        doctor.pacientesIDs[i] = doctor.pacientesIDs[i + 1];
    }
    doctor.capacidadPacientes--;
    doctor.pacientesIDs[doctor.capacidadPacientes] = 0; // Limpiar el último elemento
    doctor.fechaModificacion = time(NULL);

    // 4. Sobrescribir el registro del Doctor actualizado en disco
    if (actualizarDoctor(doctor)) {
        std::cout << "Paciente ID " << idPaciente << " removido del Dr. " << doctor.nombre << ".\n";
        return true;
    }
    
    std::cerr << "Error al actualizar el registro del doctor en disco tras la remoción.\n";
    return false;
}
void listarTodosDoctores() {
    const char* nombreArchivo = "doctores.bin";
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de doctores para listado." << std::endl;
        return;
    }
    
    // 1. Leer el Header para obtener la cantidad total de registros.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 
    
    // Configuración de la tabla
    std::cout << "\n+-------------------------------------------------------------------+\n";
    std::cout << "| " << std::setw(65) << std::left << "LISTA DE DOCTORES ACTIVOS" << " |\n";
    std::cout << "+-----+------------------------+----------------+---------------------+\n";
    std::cout << "| ID  | NOMBRE COMPLETO        | CEDULA         | ESPECIALIDAD        |\n";
    std::cout << "+-----+------------------------+----------------+---------------------+\n";

    Doctor temp; // Variable temporal para contener UN solo registro a la vez
    int activosEncontrados = 0;
    
    // 2. Iterar sobre la cantidad TOTAL de registros en el archivo.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        // Leer el siguiente registro del archivo de manera secuencial.
        archivo.read((char*)&temp, sizeof(Doctor));
        
        // 3. Comprobar que el registro no esté eliminado.
        if (!temp.eliminado) {
            // 4. Mostrar el registro activo con formato.
            std::cout << "| " << std::setw(3) << temp.id << " | "
                      << std::setw(22) << std::left << temp.nombre << " | "
                      << std::setw(14) << temp.cedula << " | "
                      << std::setw(19) << temp.especialidad << " |\n";
            activosEncontrados++;
        }
        
        if (archivo.fail()) break;
    }
    
    std::cout << "+-----+------------------------+----------------+---------------------+\n";
    std::cout << "Total de doctores listados: " << activosEncontrados 
              << " (Activos según Header: " << header.registrosActivos << ")" << std::endl;
              
    archivo.close();
}


bool eliminarDoctor(Hospital* h, int id) {
    const char* nombreArchivo = "doctores.bin";
    
    // 1. Obtener el Doctor y verificar existencia.
    Doctor d = obtenerDoctorPorID(id);
    
    // Si el ID es 0 o ya está marcado como eliminado, no existe o es redundante.
    if (d.id == 0 || d.eliminado) {
        std::cerr << "Error: Doctor ID " << id << " no encontrado o ya estaba eliminado.\n";
        return false;
    }

    // 2. Aplicar el borrado LÓGICO (cambiar la bandera).
    d.eliminado = true;
    d.fechaModificacion = time(NULL);

    if (!actualizarDoctor(d)) {
        std::cerr << "Error: No se pudo sobrescribir el registro marcado como eliminado.\n";
        return false;
    }

    ArchivoHeader header = leerHeader(nombreArchivo);
    header.registrosActivos--; // Disminuir el contador de activos
    actualizarHeader(nombreArchivo, header);
    
    std::cout << "Doctor ID " << id << " eliminado logicamente. Registros activos actualizados.\n";
    return true;
}

//Gestion de citas
bool verificarConflictoCita(int idDoctor, const char* fecha, const char* hora) {
    std::ifstream archivo("citas.bin", std::ios::binary);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    Cita temp;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // El conflicto solo aplica a citas activas, pendientes y del mismo doctor/fecha/hora
        if (!temp.eliminado && temp.doctorId == idDoctor && 
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
        std::cout << "Ya existe una cita con el doctor en esa fecha y hora.\n";
        return false;
    }

    // 2. Crear y llenar la estructura Cita
    Cita nuevaCita;
    nuevaCita.id = h->siguienteIdCita;
    nuevaCita.id = idPaciente;
    nuevaCita.id = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha));
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora));
    strcpy(nuevaCita.motivo, "Cita agendada por usuario"); // Puedes cambiar esto
    strcpy(nuevaCita.estado, "pendiente");
    nuevaCita.atendida = false;
    nuevaCita.eliminado = false; 
    nuevaCita.consultaID = -1; // Sin historial asociado inicialmente

    // 3. Guardar la Cita en citas.bin (CREATE)
    std::fstream archivo(nombreArchivo, std::ios::binary | std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para agregar cita." << std::endl;
        return false;
    }
    archivo.write((char*)&nuevaCita, sizeof(Cita));
    archivo.close();

    // 4. Actualizar las Relaciones (Enlazar la Cita al Paciente y Doctor)
    // Asumimos que esta lógica ya está implementada y que obtener/actualizar... funciona.
    Paciente p = obtenerPacientePorID(idPaciente); 
    Doctor d = obtenerDoctorPorID(idDoctor); 
    
    // (Lógica de añadir el ID de la cita al array fijo p.citasIDs y d.citasIDs)
    if (p.id != 0 && p.cantidadCitas < 20) {
        p.citasIDs[p.cantidadCitas++] = nuevaCita.id;
        actualizarPaciente(p); 
    }
    if (d.id != 0 && d.cantidadCitas < 30) {
        d.citasIDs[d.cantidadCitas++] = nuevaCita.id;
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
    
    std::cout << "Cita agendada con ID: " << nuevaCita.id << "\n";
    return true;
}
bool cancelarCita_Disco(int idCita) {
    // 1. Buscar la cita y obtener su índice en disco
    int indice = buscarIndiceCitaDeID(idCita); 
    if (indice == -1) {
        std::cerr << "Error: Cita ID " << idCita << " no encontrada para cancelar.\n";
        return false;
    }

    // 2. Leer la cita de disco
    Cita cita = obtenerCitaPorIndice(indice);
    
    // 3. Modificar el estado (borrado lógico de estado)
    if (strcmp(cita.estado, "pendiente") != 0) {
        std::cerr << "Error: Cita ID " << idCita << " no está pendiente y no puede ser cancelada.\n";
        return false;
    }
    
    strcpy(cita.estado, "cancelada");
    cita.fechaModificacion = time(NULL);

    // 4. Sobrescribir el registro actualizado en citas.bin
    if (actualizarCita(cita)) {
        std::cout << "Cita ID " << idCita << " cancelada exitosamente.\n";
        // Nota: Si usas la bandera 'eliminado' para cancelada, 
        // debes también actualizar el header.registrosActivos
        return true;
    }

    std::cerr << "Error al actualizar el registro de la cita en disco.\n";
    return false;
}

#include <iostream>
#include <fstream>
#include <iomanip> // Necesario para setw y left
#include <cstring>

// Asumimos que las estructuras Cita y ArchivoHeader están definidas.

void listarCitasPorPaciente(int idPaciente) {
    const char* nombreArchivo = "citas.bin";
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir citas.bin para listado.\n";
        return;
    }
    
    // Leer el encabezado para saber cuántas citas leer.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    std::cout << "\n+-------+----------+--------+---------+---------------------+\n";
    std::cout << "| " << std::setw(52) << std::left << "CITAS DEL PACIENTE ID " << idPaciente << " |\n";
    std::cout << "+-------+----------+--------+---------+---------------------+\n";
    std::cout << "| ID    | Fecha    | Hora   | Dr. ID  | Estado              |\n";
    std::cout << "+-------+----------+--------+---------+---------------------+\n";

    Cita temp;
    bool encontrado = false;
    
    // Iteración secuencial sobre el archivo
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // Comprobar borrado lógico y el ID del paciente
        if (!temp.eliminado && temp.pacienteId == idPaciente) {
            std::cout << "| " << std::setw(5) << temp.id 
                      << " | " << std::setw(8) << temp.fecha 
                      << " | " << std::setw(6) << temp.hora 
                      << " | " << std::setw(7) << temp.doctorId 
                      << " | " << std::setw(19) << std::left << temp.estado << " |\n";
            encontrado = true;
        }
        if (archivo.fail()) break;
    }
    
    std::cout << "+-------+----------+--------+---------+---------------------+\n";
    if (!encontrado) {
         std::cout << "| " << std::setw(52) << std::left << "No se encontraron citas activas para este paciente." << " |\n";
         std::cout << "+-------+----------+--------+---------+---------------------+\n";
    }
    archivo.close();
}

void listarCitasPorDoctor(int idDoctor) {
    const char* nombreArchivo = "citas.bin";
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir citas.bin para listado.\n";
        return;
    }
    
    // Leer el encabezado para saber cuántas citas leer.
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader)); 

    std::cout << "\n+-------+----------+--------+---------+---------------------+\n";
    std::cout << "| " << std::setw(52) << std::left << "CITAS DEL DOCTOR ID " << idDoctor << " |\n";
    std::cout << "+-------+----------+--------+---------+---------------------+\n";
    std::cout << "| ID    | Fecha    | Hora   | Pcte. ID| Estado              |\n";
    std::cout << "+-------+----------+--------+---------+---------------------+\n";

    Cita temp;
    bool encontrado = false;
    
    // Iteración secuencial sobre el archivo
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&temp, sizeof(Cita));
        
        // Comprobar borrado lógico y el ID del doctor
        if (!temp.eliminado && idDoctor == idDoctor) {
            std::cout << "| " << std::setw(5) << temp.id 
                      << " | " << std::setw(8) << temp.fecha 
                      << " | " << std::setw(6) << temp.hora 
                      << " | " << std::setw(7) << temp.idPaciente 
                      << " | " << std::setw(19) << std::left << temp.estado << " |\n";
            encontrado = true;
        }
        if (archivo.fail()) break;
    }
    
    std::cout << "+-------+----------+--------+---------+---------------------+\n";
    if (!encontrado) {
         std::cout << "| " << std::setw(52) << std::left << "No se encontraron citas activas para este doctor." << " |\n";
         std::cout << "+-------+----------+--------+---------+---------------------+\n";
    }
    archivo.close();
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

void destruirHospital(Hospital* hospital) {
    std::cout << "\nGuardando configuracion global del hospital...\n";
    
    if (guardarDatosHospital(hospital)) {
        std::cout << "Estado global guardado exitosamente.\n";
    } else {
        std::cerr << "ADVERTENCIA: Fallo al guardar el estado global del hospital. Los IDs podrian resetearse.\n";
    }

    std::cout << "Destruccion de memoria en RAM completada.\n";
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
        cout << "| 6. Cancelar Cita                     |\n"; // REUBICADO
        cout << "| 7. Listar doctores                   |\n"; // REUBICADO
        cout << "| 8. Listar pacientes                  |\n"; // REUBICADO
        cout << "| 9. Buscar paciente por Cedula        |\n"; // REUBICADO
        cout << "| 10. Buscar doctor por Especialidad   |\n"; // REUBICADO
        cout << "| 11. Buscar pacientes por Nombre      |\n"; // REUBICADO
        cout << "| 12. Eliminar paciente (Lógico)       |\n"; // REUBICADO
        cout << "| 13. Eliminar doctor (Lógico)         |\n"; // REUBICADO
        cout << "| 0. Salir y Guardar                   |\n";
        cout << "+--------------------------------------+\n";
        opcion = obtenerEntero("Seleccione una opcion: ");

        // NO SE NECESITA cin.ignore() aquí porque obtenerEntero ya lo hace.

        switch (opcion) {
            case 1: { // Registrar paciente (Ahora usa la versión de disco)
                agregarPaciente(&hospital); // La función pide los datos internamente
                break;
            }
            case 2: { // Registrar doctor (Ahora usa la versión de disco)
                agregarDoctor(&hospital); // La función pide los datos internamente
                break;
            }
            case 3: { // Agendar cita (Ahora usa la versión de disco)
                int idPaciente = obtenerEntero("ID del paciente: ");
                int idDoctor = obtenerEntero("ID del doctor: ");
                char fecha[11], hora[6];
                cout << "Fecha (DD/MM/AAAA): "; cin.getline(fecha, 11);
                cout << "Hora (HH:MM): "; cin.getline(hora, 6);
                agregarCita(&hospital, idPaciente, idDoctor, fecha, hora);
                break;
            }
            case 4: { // Atender cita (Requiere ID de cita o fecha/doctor)
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
            case 5: { // Mostrar historial
                int idPaciente = obtenerEntero("Ingrese el ID del paciente para ver historial: ");
                mostrarHistorial(idPaciente); // Función de disco
                break;
            }
            case 6: { // Cancelar Cita (Opción 6 original era listar doctores)
                int idCancelar = obtenerEntero("Ingrese el ID de la cita que desea cancelar: ");
                cancelarCita_Disco(idCancelar); // Función de disco
                break;
            }
            case 7: // Listar doctores (Opción 7 original era listar pacientes)
                listarTodosDoctores(); // Función de disco
                break;
            case 8: // Listar pacientes (Opción 8 original era buscar paciente)
                listarTodosPacientes(); // Función de disco
                break;
            case 9: { // Buscar paciente por cédula
                char cedula[21];
                cout << "Ingrese la cedula del paciente: ";
                cin.getline(cedula, 21);
                Paciente p = obtenerPacientePorCedula(cedula); // Función de disco
                if (!p.eliminado && p.id != 0) {
                    cout << "Paciente encontrado: " << p.nombre << " " << p.apellido << ", Edad: " << p.edad << "\n";
                } else {
                    cout << "Paciente no encontrado.\n";
                }
                break;
            }
            case 10: { // Buscar doctor por especialidad
                char especialidad[51];
                cout << "Ingrese la especialidad a buscar: ";
                cin.getline(especialidad, 51);
                buscarDoctoresPorEspecialidad(especialidad); // Función de disco
                break;
            }
            case 11: { // Buscar pacientes parciales
                char nombreParcial[51];
                cout << "Ingrese parte del nombre del paciente: ";
                cin.getline(nombreParcial, 51);
                buscarPacientesPorNombreParcial(nombreParcial); // Función de disco
                break;
            }
            case 12: { // Eliminar paciente (Lógico)
                int idPaciente = obtenerEntero("ID del paciente a ELIMINAR logicamente: ");
                eliminarPaciente(&hospital, idPaciente); // Función de disco
                break;
            }
            case 13: { // Eliminar doctor (Lógico)
                int idDoctor = obtenerEntero("ID del doctor a ELIMINAR logicamente: ");
                eliminarDoctor(&hospital, idDoctor); // Función de disco
                break;
            }
            case 0:
                cout << "Gracias por usar el sistema del hospital. Guardando estado...\n";
                break;
            default:
                cout << "Opción no válida. Intente de nuevo.\n";
                break;
        }
        
        // Pausar y limpiar solo si no es la opción de salir
        if (opcion != 0) {
            cout << "\nPresione ENTER para continuar...\n";
            cin.get();
            system("cls || clear");
        }

    } while (opcion != 0);

    // 2. GUARDAR ESTADO GLOBAL Y LIMPIEZA
    destruirHospital(&hospital); // Función que ahora guarda el estado final en hospital.bin

    return 0;
}

