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
    int id = 0;
    char fecha[11];
    char hora[6];
    char diagnostico[500];
    char tratamiento[500];
    char observaciones[500];
    int idPaciente;
    int idDoctor;
    int idconsulta;
    double costo;

    int siguienteConsultaID;
    bool eliminado;
};

// Estructura Paciente
struct Paciente {
    int id = 0;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo [2];
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];
    bool activo;
    int citasIDs[20];

    
    int cantidadConsultas;
    int primerConsultaID;
    
    

    
    int cantidadCitas;
    

    char alergias[500];
    char observaciones[500];

    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// Estructura Doctor 
struct Doctor {
    int id = 0;
    char nombre[50];
    int edad;
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[100];
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
    bool atendida;
    

    char observaciones[200];
    int ConsultaID;
    bool confirmada;

    int consultasID;
    int activa;

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
struct HospitalMetadata {
    int nextPacienteID = 1;
    int nextDoctorID = 1;
    int nextCitaID = 1;
    int nextConsultaID = 1;
    int cantidadDoctoresActivos = 0;
    int cantidadPacientesActivos = 0;
};
HospitalMetadata hospital;

ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header;
    // Inicializar el header con un valor inválido por defecto (ej. -1)
    header.proximoID = -1; 
    
    // Abrir en modo de lectura binaria
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo para lectura del Header: " << nombreArchivo << endl;
        return header;
    }
    
    // Leer exactamente el tamaño del Header desde el inicio (posición 0)
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    
    archivo.close();
    return header;
}
int obtenerEntero(const char* prompt) {
    int valor;
    cout << prompt;
    while (!(cin >> valor)) {
        cout << "Entrada invalida. Ingrese un numero: ";
        cin.clear();
        // Limpiar el buffer de entrada en caso de error
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
     

    return valor;
}
void obtenerCadena(char* destino, size_t tamano) {
    
    if (cin.peek() == '\n') { 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    // Lectura real de la línea
    cin.getline(destino, tamano);

    


}
float obtenerFlotante() {
    float valor;
    
    // Intenta leer el valor flotante
    cin >> valor;

    // 1. Comprobar si hubo un fallo en la lectura 
    if (cin.fail()) {
        cerr << " Entrada inválida. Se esperaba un número. Usando 0.00." << endl;
        valor = 0.0f; // Asigna un valor seguro (cero)
        
        
        cin.clear();
    }
    
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return valor;
}
bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header) {
    
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo para actualizar el Header: " << nombreArchivo << endl;
        return false;
    }
    
    // Posicionarse al inicio del archivo para la escritura
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
        cerr << " No se pudo crear o abrir el archivo para inicializar: " 
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
bool inicializarSistema() {
    // Abre el archivo en modo LECTURA binaria
    ifstream file("hospital.bin", ios::in | ios::binary);
    
    if (file.is_open()) {
        // Mueve el puntero al final para verificar si el archivo tiene contenido
        file.seekg(0, ios::end);
        long tamano = file.tellg();
        
        if (tamano >= sizeof(HospitalMetadata)) {
            // Si tiene contenido suficiente, mueve el puntero al inicio (byte 0)
            file.seekg(0, ios::beg); 
            
            // Lee la estructura completa del disco a la variable global 'hospital'
            file.read(reinterpret_cast<char*>(&hospital), sizeof(HospitalMetadata));
            
            cout << "\n✅ Sistema inicializado. Metadatos cargados de hospital.bin." << endl;
        } else {
            // Archivo vacío o corrupto, inicializa por defecto
            hospital = HospitalMetadata{};
        }
        
        file.close();
        return true;
    } 
    
    
    hospital = HospitalMetadata{};
    
    return true; 
}

bool guardarMetadatos() {
    
    ofstream file("hospital.bin", ios::out | ios::binary);
    
    if (file.is_open()) {
        // Escribe la estructura completa de metadatos (la variable 'hospital')
        file.write(reinterpret_cast<const char*>(&hospital), sizeof(HospitalMetadata));
        
        file.close();
        
        
    } else {
        // Es un error crítico si el sistema no puede guardar el Header.
        cerr << " No se pudieron guardar los metadatos en hospital.bin." << endl;
    }
}
Paciente obtenerPacientePorCedula(const char* cedula) {
    const char* nombreArchivo = "pacientes.bin";
    Paciente pEncontrado = {0}; // Estructura vacía para retornar si no se encuentra
    
    std::ifstream archivo(nombreArchivo, std::ios::binary); 
    
    if (!archivo.is_open()) {
        std::cerr << " No se pudo abrir el archivo de pacientes para busqueda por cedula." << std::endl;
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
bool validarEmail(const char* email) {
    // 1. Verificar nulo o vacío
    if (email == nullptr || strlen(email) == 0) return false;
    
    // 2. Verificar el límite de tamaño fijo (Asumiendo 100 caracteres)
    if (strlen(email) > 100) {
        cerr << "El email excede el límite de 100 caracteres." << endl;
        return false;
    }
    if (strchr(email, '@') == nullptr) {
        cerr << "El email debe contener el símbolo '@'." << endl;
        return false;
    }
    
    return true;
}
bool agregarPaciente(Paciente nuevo) {
    fstream archivo("pacientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;

    // Leer header
    ArchivoHeader header;
    archivo.read((char*)&header, sizeof(ArchivoHeader));

    // Asignar ID
    nuevo.id = header.proximoID;
    nuevo.eliminado = false;

    // Posicionarse al final
    archivo.seekp(0, ios::end);
    archivo.write((char*)&nuevo, sizeof(Paciente));

    // Actualizar header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));

    archivo.close();
    return true;
}

int buscarIndiceDeID(int id) {
    const char* nombreArchivo = "pacientes.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo de pacientes para busqueda." << endl;
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


Paciente buscarPacientePorID_BIN(int id) {
    Paciente paciente; // Estructura local, inicializada con id=0 por defecto

    // 1. Validar ID
    if (id <= 0) {
        return paciente; // Devuelve paciente con id=0
    }

    // 2. Abrir el archivo en modo lectura binaria
    fstream file("pacientes.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo pacientes.bin para lectura." << endl;
        return paciente;
    }

    
    long pos = (long)(id - 1) * sizeof(Paciente);

    // 4. Verificar si la posición es válida dentro del archivo
    file.seekg(0, ios::end);         // Mover puntero al final
    long tamanoArchivo = file.tellg(); // Obtener tamaño total en bytes
    
    if (pos >= tamanoArchivo) {
        // El ID está fuera del rango de registros existentes en el disco.
        file.close();
        return paciente;
    }

    // 5. Mover el puntero de lectura al inicio del registro deseado
    file.seekg(pos);

    // 6. Leer SOLO el registro completo del disco a la variable local 'paciente'
    if (file.read(reinterpret_cast<char*>(&paciente), sizeof(Paciente))) {
        // 7. Verificar que el registro leído corresponde al ID buscado y esté activo
        if (paciente.id == id && paciente.activo) {
            file.close();
            return paciente; // Éxito: Registro encontrado y devuelto
        } 
        // Si el ID no coincide o no está activo, se considera "no encontrado"
    } else {
        cerr << " al leer el registro en la posición calculada." << endl;
    }

    // 8. Cerrar el archivo y retornar la estructura vacía si no se encontró o estaba inactiva
    file.close();
    return Paciente(); // Retorna un objeto Paciente con id=0 (por defecto)
}

bool actualizarPaciente(Paciente pModificado) {
    const char* nombreArchivo = "pacientes.bin";
    
    
    int indice = buscarIndiceDeID(pModificado.id);
    if (indice == -1) {
        cerr << " Paciente con ID " << pModificado.id << " no encontrado para actualizar." << endl;
        return false;
    }

    // 2. Calcular la posición exacta en bytes.
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    
    // 3. Abrir el archivo en modo de lectura y escritura binaria.
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo para actualizar paciente." << endl;
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
bool modificarPaciente_BIN(const Paciente& pModificado) {
    // 1. Validar que el ID del paciente es positivo
    if (pModificado.id <= 0) {
        cerr << " ID de paciente inválido para modificación." << endl;
        return false;
    }
    
    
    fstream file("pacientes.bin", ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo pacientes.bin para modificación." << endl;
        return false;
    }

    
    long pos = (long)(pModificado.id - 1) * sizeof(Paciente);

    // 4. Mover el puntero de ESCRITURA al inicio del registro deseado
    file.seekp(pos);

    // 5. Sobrescribir SOLO el registro completo del disco con los nuevos datos
    if (file.write(reinterpret_cast<const char*>(&pModificado), sizeof(Paciente))) {
        file.close();
        // cout << "Registro de Paciente ID " << pModificado.id << " modificado con éxito." << endl; // Opcional
        return true;
    } else {
        cerr << " Falló la escritura del registro de Paciente ID " << pModificado.id << "." << endl;
        file.close();
        return false;
    }
}
   bool eliminarPaciente_BIN(int idPaciente) {
    // 1. Obtener el registro del paciente usando Acceso Aleatorio
    Paciente p = buscarPacientePorID_BIN(idPaciente); 

    // 2. Verificar existencia y estado
    if (p.id == 0) {
        cerr << " Paciente ID " << idPaciente << " no encontrado." << endl;
        return false;
    }
    
    // El campo 'activo' (en lugar de 'eliminado') es la bandera de eliminación lógica.
    if (!p.activo) {
        cerr << " Paciente ID " << idPaciente << " ya estaba inactivo." << endl;
        return false;
    }

    
    if (p.cantidadCitas > 0) {
        cout << " El paciente ID " << idPaciente << " tiene " << p.cantidadCitas 
             << " citas agendadas/pendientes. Cancele las citas primero." << endl;
        return false;
    }

    // 4. Aplicar la Eliminación LÓGICA en la copia en RAM
    p.activo = false;
    
    if (!modificarPaciente_BIN(p)) {
        cerr << " No se pudo sobrescribir el registro en pacientes.bin." << endl;
        return false;
    }

    // 6. Actualizar el Header/Metadatos (Variable Global)
    if (hospital.cantidadPacientesActivos > 0) {
        hospital.cantidadPacientesActivos--;
    }
    
    // 7. Guardar el Header actualizado en el archivo hospital.bin
    guardarMetadatos();

    cout << "Paciente ID " << idPaciente << " eliminado lógicamente y metadatos actualizados." << endl;
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
        cerr << " No se pudo abrir el archivo de pacientes para listado." << endl;
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
        cerr << " No se pudo abrir el archivo de pacientes para busqueda parcial." << endl;
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

void mostrarHistorial_BIN(int idPaciente) {
    // 1. Obtener el registro del Paciente (Acceso Aleatorio)
    Paciente paciente = buscarPacientePorID_BIN(idPaciente);

    if (paciente.id == 0 || !paciente.activo) {
        cerr << " Paciente con ID " << idPaciente << " no encontrado o inactivo." << endl;
        return;
    }

    // El ID 0 se usa como puntero nulo para indicar el final de la lista.
    if (paciente.primerConsultaID == 0) {
        cout << "El paciente " << paciente.nombre << " no tiene historial médico registrado." << endl;
        return;
    }

    // 2. Abrir el archivo de historial para lectura binaria
    fstream file("historiales.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo historiales.bin para lectura." << endl;
        return;
    }

    int currentID = paciente.primerConsultaID;
    HistorialMedico consulta;

    cout << "\n+---------------------------------------------------------------------------------------------+\n";
    cout << " HISTORIAL MEDICO DEL PACIENTE ID: " << idPaciente << " (" << paciente.nombre << " " << paciente.apellido << ")\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";
    cout << "| ID |    Fecha    |  Hora  |        Diagnostico        | ID Doctor |   Costo   |\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";
    
    // 3. Recorrer la lista enlazada binaria
    while (currentID != 0) {
        
        // 3.a. Calcular la posición física del registro (Acceso Aleatorio)
        long pos = (long)(currentID - 1) * sizeof(HistorialMedico);

        // 3.b. Mover el puntero de lectura al inicio del registro
        file.seekg(pos);

        // 3.c. Leer SOLO ese registro
        if (!file.read(reinterpret_cast<char*>(&consulta), sizeof(HistorialMedico))) {
            cerr << " Error al leer el registro ID " << currentID << ". Terminando historial." << endl;
            break; 
        }

        // 3.d. Mostrar la información con formato tabular
        if (!consulta.eliminado) {
             cout << "| " << setw(2) << consulta.idconsulta 
                 << " | " << setw(11) << consulta.fecha
                 << " | " << setw(6) << consulta.hora
                 << " | " << setw(27) << left << consulta.diagnostico
                 << " | " << setw(9) << right << consulta.idDoctor // Usamos right para IDs
                 << " | $" << setw(7) << right << fixed << setprecision(2) << consulta.costo << " |\n";
        }
        
        // 3.e. Avanzar al siguiente nodo de la lista
        currentID = consulta.siguienteConsultaID;
    }

    // 4. Cerrar el archivo
    file.close();
    cout << "+---------------------------------------------------------------------------------------------+\n";
}
HistorialMedico obtenerHistorialMedicoPorID_BIN(int id) {
    HistorialMedico historial{}; // Inicialización: id=0 por defecto

    // 1. Validar ID
    if (id <= 0) {
        return historial; 
    }

    // 2. Abrir el archivo en modo lectura binaria
    fstream file("historiales.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo historiales.bin para lectura." << endl;
        return historial;
    }

    
    long pos = (long)(id - 1) * sizeof(HistorialMedico);

    // 4. Verificar si la posición es válida dentro del archivo
    file.seekg(0, ios::end);         // Mover puntero al final
    long tamanoArchivo = file.tellg(); // Obtener tamaño total en bytes
    
    // Si la posición calculada está fuera del límite del archivo, el registro no existe
    if (pos >= tamanoArchivo) {
        file.close();
        return historial;
    }

    // 5. Mover el puntero de lectura al inicio del registro deseado
    file.seekg(pos);

    // 6. Leer SOLO el registro completo
    if (file.read(reinterpret_cast<char*>(&historial), sizeof(HistorialMedico))) {
        // 7. Verificar que el registro leído corresponde al ID buscado
        if (historial.id == id) {
            file.close();
            return historial; // Éxito: Registro encontrado y devuelto
        } 
    } else {
        // Fallo en la lectura
        cerr << " Error al leer el registro ID " << id << " en la posición calculada." << endl;
    }

    // 8. Cerrar el archivo y retornar el objeto vacío si no se encontró
    file.close();
    return HistorialMedico{}; 
}

HistorialMedico obtenerUltimaConsultaDeDisco(Paciente paciente) {
    // Estructura vacía, usada para indicar que no se encontró nada.
    HistorialMedico hVacia = {}; 
    
    // 1. Verificar si hay historial registrado
    if (paciente.cantidadConsultas == 0 || paciente.primerConsultaID <= 0) {
        return hVacia;
    }

    int idActual = paciente.primerConsultaID;
    HistorialMedico consultaActual;

    // 2. Recorrer la lista enlazada en el archivo historiales.bin
    while (idActual != -1) {
        // Leer el registro de la consulta actual del archivo
        consultaActual = obtenerHistorialMedicoPorID_BIN(idActual); 
        
        // 3. Comprobar si este es el último nodo (enlace = -1)
        if (consultaActual.siguienteConsultaID == -1) {
            return consultaActual; // ¡Último nodo encontrado!
        }
        
        // 4. Moverse al siguiente nodo en el disco
        idActual = consultaActual.siguienteConsultaID;
        
        // Manejo de error si se encuentra un enlace roto (id 0 o loop inesperado)
        if (idActual == 0) { 
            std::cerr << " Se detectó un enlace roto en el historial médico." << std::endl;
            break;
        }
    }

    return hVacia; // Devolver estructura vacía si el recorrido falla
}



bool crearDoctor_BIN() {
    // 1. Inicializar la estructura Doctor (constructor por defecto)
    Doctor nuevoDoctor{}; 
    
    // 2. Asignar el ID único del sistema
    nuevoDoctor.id = hospital.nextDoctorID;
    
    // 3. Recolectar datos del usuario
    cout << "\n--- REGISTRANDO NUEVO DOCTOR ID " << nuevoDoctor.id << " ---\n";
    
    // Aquí van tus llamadas a funciones de input
    cout << "Nombre del Doctor: ";
    obtenerCadena(nuevoDoctor.nombre, sizeof(nuevoDoctor.nombre));

    cout << "Especialidad: ";
    obtenerCadena(nuevoDoctor.especialidad, sizeof(nuevoDoctor.especialidad));

    nuevoDoctor.aniosExperiencia= obtenerEntero("Numero de Licencia: ");
    
    nuevoDoctor.activo = true;           
    nuevoDoctor.capacidadPacientes = 0;    
   

    
    ofstream file("doctores.bin", ios::out | ios::app | ios::binary);
    
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo doctores.bin para escritura." << endl;
        return false;
    }

    
    file.write(reinterpret_cast<const char*>(&nuevoDoctor), sizeof(Doctor));
    file.close();

    
    hospital.nextDoctorID++;
    hospital.cantidadDoctoresActivos++;

    // 8. Persistir los metadatos actualizados en hospital.bin
    guardarMetadatos();

    cout << " Doctor ID " << nuevoDoctor.id << " registrado exitosamente." << endl;
    return true;
}

bool modificarDoctor_BIN(const Doctor& dModificado) {
    // 1. Validar que el ID del doctor es positivo
    if (dModificado.id <= 0) {
        cerr << " ID de doctor inválido para modificación." << endl;
        return false;
    }
    
    
    fstream file("doctores.bin", ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo doctores.bin para modificación." << endl;
        return false;
    }

    
    long pos = (long)(dModificado.id - 1) * sizeof(Doctor);

    // 4. Mover el puntero de ESCRITURA al inicio del registro deseado
    file.seekp(pos);

    // 5. Sobrescribir SOLO el registro completo del disco con los nuevos datos
    if (file.write(reinterpret_cast<const char*>(&dModificado), sizeof(Doctor))) {
        file.close();
        // cout << "Registro de Doctor ID " << dModificado.id << " modificado con éxito." << endl; // Opcional
        return true;
    } else {
        cerr << " Falló la escritura del registro de Doctor ID " << dModificado.id << "." << endl;
        file.close();
        return false;
    }
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
    Doctor d {}; 
    
    if (indice < 0) return d;

    ifstream archivo("doctores.bin", std::ios::binary);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir doctores.bin para lectura aleatoria." << endl;
        return d;
    }

    
    long posicion = calcularPosicion(indice, sizeof(Doctor));
    
    
    archivo.seekg(posicion, ios::beg);
    archivo.read((char*)&d, sizeof(Doctor));
    
    archivo.close();
    return d;
}


Doctor obtenerDoctorPorID(int id) {
    
    int indice = buscarIndiceDoctorDeID(id);
    
    
    if (indice != -1) {
        return obtenerDoctorPorIndice(indice);
    }
    

    return Doctor{}; 
}

Doctor obtenerDoctorPorCedula_BIN(const char* cedulaBuscada) {
    Doctor dEncontrado{}; 
    
    ifstream archivo("doctores.bin", ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo doctores.bin." <<endl;
        return dEncontrado;
    }
    
    int totalRegistrosCreados = hospital.nextDoctorID - 1; 

    Doctor temp{}; 
    
    for (int id = 1; id <= totalRegistrosCreados; id++) {
        long pos = (long)(id - 1) * sizeof(Doctor);
        archivo.seekg(pos);

        // 3. Leer un registro
        if (!archivo.read(reinterpret_cast<char*>(&temp), sizeof(Doctor))) {
            // Si la lectura falla, salimos.
            break; 
        }
        if (temp.id > 0 && temp.activo) { 
            // Comparación de cédula
            if (strcmp(temp.cedula, cedulaBuscada) == 0) {
                archivo.close();
                return temp; 
            }
        }
    }
    
    archivo.close();
    return dEncontrado; // No se encontró ningún doctor activo con esa cédula
}
Doctor obtenerDoctorPorEspecialidad_BIN(const char* especialidadBuscada) {
    Doctor dEncontrado{}; 
    
    ifstream archivo("doctores.bin", ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo doctores.bin." << endl;
        return dEncontrado;
    }
    
    int totalRegistrosCreados = hospital.nextDoctorID - 1; 

    Doctor temp{}; 
    
    for (int id = 1; id <= totalRegistrosCreados; id++) {
        long pos = (long)(id - 1) * sizeof(Doctor);
        archivo.seekg(pos);

        if (!archivo.read(reinterpret_cast<char*>(&temp), sizeof(Doctor))) {
            break; 
        }
        
        if (temp.id > 0 && temp.activo) { 
            if (strcmp(temp.especialidad, especialidadBuscada) == 0) {
                archivo.close();
                return temp; 
            }
        }
    }
    
    archivo.close();
    return dEncontrado; 
}
bool asignarPacienteADoctor_Disco(int idDoctor, int idPaciente) {

    Doctor doctor = obtenerDoctorPorID(idDoctor);
    if (doctor.id == 0 || doctor.eliminado) {
        cerr << " Doctor ID " << idDoctor << " no encontrado.\n";
        return false;
    }
    
    
    const int CAPACIDAD_MAXIMA = 50; 

    
    if (doctor.capacidadPacientes >= CAPACIDAD_MAXIMA) {
        cerr << " El Dr. " << doctor.nombre << " ha alcanzado el límite de pacientes (" << CAPACIDAD_MAXIMA << ").\n";
        return false;
    }

    for (int i = 0; i < doctor.capacidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            cerr << " Paciente ID " << idPaciente << " ya estaba asignado a este doctor.\n";
            return false;
        }
    }

    // 3. Añadir el ID del Paciente al array fijo
    doctor.pacientesIDs[doctor.capacidadPacientes++] = idPaciente;
    doctor.fechaModificacion = time(NULL); // Actualizar metadata

    // 4. Sobrescribir el registro del Doctor en doctores.bin (Escritura con Acceso Aleatorio)
    if (modificarDoctor_BIN(doctor)) {
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
        cerr << " Doctor ID " << idDoctor << " no encontrado.\n";
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
        Paciente p = buscarPacientePorID_BIN(idPaciente); 
        
        
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

    
    if (modificarDoctor_BIN(doctor)) {
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
        cerr << " No se pudo abrir el archivo de doctores para listado." <<endl;
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
    

    return (*s1 == *s2) ? 0 : 1; 
}
void buscarDoctoresPorEspecialidad_BIN(const char* especialidadBuscada) {
    ifstream archivo("doctores.bin", ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir el archivo de doctores (doctores.bin)." << endl;
        return;
    }
    
    Doctor temp{};
    int encontrados = 0;
    
    cout << "\n--- Doctores en la especialidad: " << especialidadBuscada << " ---\n";

    int totalRegistrosCreados = hospital.nextDoctorID - 1; 
    
    
    for (int id = 1; id <= totalRegistrosCreados; id++) {

        long pos = (long)(id - 1) * sizeof(Doctor);
        archivo.seekg(pos);

        // 3. Leer un registro
        if (!archivo.read(reinterpret_cast<char*>(&temp), sizeof(Doctor))) {
            
            break; 
        }
        if (temp.activo) { 
            
            if (strcmp(temp.especialidad, especialidadBuscada) == 0) {
                
                cout << "ID: " << temp.id 
                     << ", Nombre: " << temp.nombre 
                     << ", Especialidad: " << temp.especialidad 
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
Doctor buscarDoctorPorID_BIN(int id) {
    Doctor doctor{}; // Inicialización de valor: garantiza que id=0 por defecto

    // 1. Validar ID
    if (id <= 0) {
        return doctor; 
    }

    // 2. Abrir el archivo en modo lectura binaria
    fstream file("doctores.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo doctores.bin para lectura." << endl;
        return doctor;
    }

    
    long pos = (long)(id - 1) * sizeof(Doctor);

    
    file.seekg(0, ios::end);         // Mover puntero al final
    long tamanoArchivo = file.tellg(); // Obtener tamaño total en bytes
    
    
    if (pos >= tamanoArchivo) {
        file.close();
        return doctor;
    }

   
    file.seekg(pos);

    // 6. Leer SOLO el registro completo
    if (file.read(reinterpret_cast<char*>(&doctor), sizeof(Doctor))) {
        // 7. Verificar que el registro leído corresponde al ID buscado y esté activo
        if (doctor.id == id && doctor.activo) {
            file.close();
            return doctor; // Éxito: Registro encontrado y devuelto
        } 
    } else {
        
        cerr << " Error al leer el registro ID " << id << " en la posición calculada." << endl;
    }

    
    file.close();
    return Doctor{}; 
}

HistorialMedico buscarHistorialMedicoPorID_BIN(int id) {
    HistorialMedico historial{}; // Inicialización: id=0 por defecto

    // 1. Validar ID
    if (id <= 0) {
        return historial; 
    }

    // 2. Abrir el archivo en modo lectura binaria
    fstream file("historiales.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo historiales.bin para lectura." << endl;
        return historial;
    }

    // 3. Calcular la posición del registro (Acceso Aleatorio)
    
    long pos = (long)(id - 1) * sizeof(HistorialMedico);

    // 4. Verificar si la posición es válida dentro del archivo
    file.seekg(0, ios::end);         
    long tamanoArchivo = file.tellg(); 
    
    // Si la posición calculada está fuera del límite del archivo, el registro no existe
    if (pos >= tamanoArchivo) {
        file.close();
        return historial;
    }

    // 5. Mover el puntero de lectura al inicio del registro deseado
    file.seekg(pos);

    // 6. Leer SOLO el registro completo
    if (file.read(reinterpret_cast<char*>(&historial), sizeof(HistorialMedico))) {
        // 7. Verificar que el registro leído corresponde al ID buscado
        if (historial.id == id) {
            file.close();
            return historial; // Éxito: Registro encontrado y devuelto
        } 
    } else {
        // Fallo en la lectura
        cerr << " Error al leer el registro ID " << id << " en la posición calculada." << endl;
    }

    // 8. Cerrar el archivo y retornar el objeto vacío si no se encontró
    file.close();
    return HistorialMedico{}; 
}

bool tieneCitasActivas(int idDoctor) {
    // 1. Abrir el archivo en modo lectura binaria
    fstream file("citas.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir citas.bin. Asumiendo que no hay citas activas." << endl;
        return false; // Si no se puede abrir, no hay pruebas de citas
    }

    Cita temp;

    // 2. Recorrido secuencial del archivo citas.bin
    while (file.read(reinterpret_cast<char*>(&temp), sizeof(Cita))) {
        
        // El ID debe ser válido (no 0, que indica un registro vacío o eliminado físicamente)
        if (temp.id != 0) { 
            
            // 3. Verificar la condición: ¿Es el doctor y está la cita activa?
            if (temp.idDoctor == idDoctor && temp.activa == true) {
                // Se encontró al menos una cita activa
                file.close();
                return true; 
            }
        }
    }

    // 4. Cerrar el archivo
    file.close();
    
    // Si se recorrió todo el archivo sin encontrar citas activas para ese doctor
    return false;
}
bool eliminarDoctor_BIN(int idDoctor) {
    // 1. Cargar el registro del Doctor usando Acceso Aleatorio
    Doctor doctor = buscarDoctorPorID_BIN(idDoctor);

    if (doctor.id == 0) {
        cout << " Doctor con ID " << idDoctor << " no encontrado." << endl;
        return false;
    }
    
    if (!doctor.activo) {
        cout << " Doctor con ID " << idDoctor << " ya estaba inactivo." << endl;
        return false;
    }
    if (tieneCitasActivas(idDoctor)) {
        cout << " El doctor ID " << idDoctor << " tiene citas pendientes. Cancele o atienda sus citas primero." << endl;
        return false;
    }
    
    // 3. Implementar la Eliminación LÓGICA en la copia en RAM
    doctor.activo = false;

    // 4. Sobrescribir SOLO el registro modificado en el archivo doctores.bin
    if (modificarDoctor_BIN(doctor)) {
        
        // 5. Actualizar Metadatos (Header)
        if (hospital.cantidadDoctoresActivos > 0) {
            hospital.cantidadDoctoresActivos--;
        }
        guardarMetadatos(); // Guarda la variable global 'hospital' en 'hospital.bin'
        
        cout << "Doctor ID " << idDoctor << " eliminado lógicamente y metadatos actualizados." << endl;
        return true;
    } else {
        cerr << " No se pudo sobrescribir el registro en el disco." << endl;
        return false;
    }
}

//Gestion de citas
bool verificarConflictoCita_BIN(int idDoctor, const char* fecha, const char* hora) {
    // Usamos fstream para evitar problemas de seek si el archivo está vacío.
    ifstream archivo("citas.bin", ios::binary);
    if (!archivo.is_open()) {
        // Si el archivo no existe o no se puede abrir, no hay conflicto.
        return false;
    }

    Cita temp{};
    
    archivo.seekg(0, ios::end);
    long tamanoArchivo = archivo.tellg();
    if (tamanoArchivo == 0) {
        archivo.close();
        return false;
    }
    
    // Mover el puntero al inicio
    archivo.seekg(0, ios::beg);
    
    // Si usas el ID siguiente como contador total de registros:
    int totalRegistros = hospital.nextCitaID - 1; // El ID más alto es el total de registros.

    for (int i = 0; i < totalRegistros; i++) {
        // Leer el registro de la posición actual
        if (!archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cita))) {
            // Si falla la lectura, es el final del archivo o un error, salimos.
            break; 
        }

        
        if (temp.id > 0 && temp.activa && !temp.atendida &&
            temp.idDoctor == idDoctor && 
            strcmp(temp.fecha, fecha) == 0 && 
            strcmp(temp.hora, hora) == 0) 
        {
            archivo.close();
            return true; // Conflicto encontrado
        }
    }
    
    archivo.close();
    return false; // No hay conflicto
}


bool crearCita_BIN() {
    // 1. Inicializar estructura Cita y asignar ID
    Cita nuevaCita{};
    int nuevoID = hospital.nextCitaID;
    nuevaCita.id = nuevoID;
    
    // 2. Obtener y validar IDs de Paciente y Doctor
    cout << "\n--- AGENDANDO CITA ID " << nuevaCita.id << " ---\n";
    
    int idPaciente = obtenerEntero("ID del Paciente: ");
    Paciente p = buscarPacientePorID_BIN(idPaciente);
    if (p.id == 0 || !p.activo) {
        cerr << " Paciente ID " << idPaciente << " no encontrado o inactivo." << endl;
        return false;
    }
    nuevaCita.idPaciente = idPaciente;
    
    int idDoctor = obtenerEntero("ID del Doctor: ");
    Doctor d = buscarDoctorPorID_BIN(idDoctor);
    if (d.id == 0 || !d.activo) {
        cerr << " Doctor ID " << idDoctor << " no encontrado o inactivo." << endl;
        return false;
    }
    nuevaCita.idDoctor = idDoctor;

    // 3. Recolectar fecha, hora y motivo
    cout << "Fecha (dd/mm/aaaa): ";
    obtenerCadena(nuevaCita.fecha, sizeof(nuevaCita.fecha));
    
    cout << "Hora (hh:mm): ";
    obtenerCadena(nuevaCita.hora, sizeof(nuevaCita.hora));
    
    cout << "Motivo de la cita: ";
    obtenerCadena(nuevaCita.motivo, sizeof(nuevaCita.motivo));

    // 4. Setear banderas iniciales
    nuevaCita.activa = true;
    nuevaCita.atendida = false;

    // 5. Escribir la Cita en el archivo citas.bin
    ofstream file("citas.bin", ios::out | ios::app | ios::binary);
    
    if (!file.is_open()) {
        cerr << " No se pudo abrir citas.bin para escritura." << endl;
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(&nuevaCita), sizeof(Cita));
    file.close();
    


    
    p.cantidadCitas++;
    
    if (!modificarPaciente_BIN(p)) {
        cerr << " Cita creada, pero falló la actualización del contador del paciente." << endl;
        
    }

    
    hospital.nextCitaID++;
    guardarMetadatos();

    cout << " Cita ID " << nuevoID << " agendada exitosamente para el Paciente ID " << idPaciente << "." << endl;
    return true;
}
Cita obtenerCitaPorIndice(int indiceBuscado);

Cita buscarCitaPorID_BIN(int id) {
    Cita cita{}; // Inicialización de valor: garantiza que id=0 por defecto

    // 1. Validar ID
    if (id <= 0) {
        return cita; 
    }

    // 2. Abrir el archivo en modo lectura binaria
    fstream file("citas.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo citas.bin para lectura." << endl;
        return cita;
    }

    // 3. Calcular la posición del registro (Acceso Aleatorio)
    // Posición = (ID - 1) * sizeof(Registro)
    long pos = (long)(id - 1) * sizeof(Cita);

    // 4. Verificar si la posición es válida dentro del archivo
    file.seekg(0, ios::end);         // Mover puntero al final
    long tamanoArchivo = file.tellg(); // Obtener tamaño total en bytes
    
    // Si la posición calculada está fuera del límite del archivo, el registro no existe
    if (pos >= tamanoArchivo) {
        file.close();
        return cita;
    }

    // 5. Mover el puntero de lectura al inicio del registro deseado
    file.seekg(pos);

    // 6. Leer SOLO el registro completo
    if (file.read(reinterpret_cast<char*>(&cita), sizeof(Cita))) {
        // 7. Verificar que el registro leído corresponde al ID buscado
        if (cita.id == id) {
            file.close();
            return cita; // Éxito: Registro encontrado y devuelto
        } 
    } else {
        // Fallo en la lectura
        cerr << " Error al leer el registro ID " << id << " en la posición calculada." << endl;
    }

    // 8. Cerrar el archivo y retornar el objeto vacío si no se encontró
    file.close();
    return Cita{}; 
}
bool crearYEnlazarConsulta_BIN(int idPaciente, HistorialMedico nuevaConsultaData) {
    // 1. Obtener el Paciente (Necesario para actualizar los punteros)
    Paciente p = buscarPacientePorID_BIN(idPaciente);

    if (p.id == 0 || !p.activo) {
        cerr << " Paciente ID " << idPaciente << " no encontrado para enlazar consulta." << endl;
        return false;
    }
    
    
    nuevaConsultaData.id = hospital.nextConsultaID;
    // El nuevo nodo SIEMPRE apunta a 0 (es el último de la lista por ahora)
    nuevaConsultaData.siguienteConsultaID = 0; 
    
  
    ofstream file("historiales.bin", ios::out | ios::app | ios::binary);
    
    if (!file.is_open()) {
        cerr << " No se pudo abrir historiales.bin para crear nueva consulta." << endl;
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(&nuevaConsultaData), sizeof(HistorialMedico));
    file.close();

    // 3. Actualizar el enlace del Paciente
    int idConsultaActualizada = nuevaConsultaData.id;
    
    if (p.primerConsultaID == 0) {
        // A. Primera consulta: El paciente apunta directamente a este ID
        p.primerConsultaID = idConsultaActualizada;
    } else {
        






    
        
        
        HistorialMedico consultaActual = buscarHistorialMedicoPorID_BIN(p.primerConsultaID);
        int idConsultaAnterior = 0;

       
        while (consultaActual.siguienteConsultaID != 0) {
            consultaActual = buscarHistorialMedicoPorID_BIN(consultaActual.siguienteConsultaID);
            
        }
        consultaActual.siguienteConsultaID = idConsultaActualizada;
    }
    p.cantidadConsultas++;

    if (!modificarPaciente_BIN(p)) {
        cerr << " No se pudo actualizar el registro del paciente con el nuevo enlace." << endl;
        // Esto es un error grave: se creó la consulta pero no se enlazó.
        return false;
    }

    // 5. Actualizar Metadatos Globales (Header)
    hospital.nextConsultaID++;
    guardarMetadatos();

    return true;
}
bool modificarCita_BIN(const Cita& cModificada);

bool atenderCita_BIN(int idCita) {
    // 1. Buscar y Validar la Cita
    Cita cita = buscarCitaPorID_BIN(idCita);
    int nuevoIdConsulta = 0;
    

    if (cita.id == 0 || !cita.activa) {
        cerr << " Cita ID " << idCita << " no encontrada o ya está inactiva/cancelada." << endl;
        return false;
    }
    
    if (cita.atendida) {
        cout << " La cita ID " << idCita << " ya fue marcada como completada." << endl;
      
        return false; 
    }

    // 2. Inicializar y Recolectar datos del Historial (Consulta)
    HistorialMedico nuevaConsulta{};
    
    // --- Recolección de Datos ---
    cout << "\n--- ATENDER CITA ID " << idCita << " ---\n";
    cout << "Ingrese el Diagnóstico: ";
    // Asumo que obtenerCadena existe y limpia el buffer.
    obtenerCadena(nuevaConsulta.diagnostico, sizeof(nuevaConsulta.diagnostico)); 

    cout << "Ingrese el Tratamiento: ";
    obtenerCadena(nuevaConsulta.tratamiento, sizeof(nuevaConsulta.tratamiento)); 

    cout << "Medicamentos (si aplica): ";
    obtenerCadena(nuevaConsulta.tratamiento, sizeof(nuevaConsulta.tratamiento)); 

    cout << "Costo Final de la consulta ($): ";
    
    nuevaConsulta.costo = obtenerFlotante();
    

    nuevaConsulta.idDoctor = cita.idDoctor; 
    strncpy(nuevaConsulta.fecha, cita.fecha, sizeof(nuevaConsulta.fecha));
    strncpy(nuevaConsulta.hora, cita.hora, sizeof(nuevaConsulta.hora));
    
    
    cita.atendida = true;
    cita.activa = false; 

    
    if (!modificarCita_BIN(cita)) {
        cerr << " No se pudo marcar la cita ID " << idCita << " como completada." << endl;
        return false;
    }

if (nuevoIdConsulta > 0) {
    // ➡️ Éxito: La consulta fue creada con el ID nuevoIdConsulta
    cout << "✅ Historial médico creado y enlazado con éxito (ID: " << nuevoIdConsulta << ").\n";
} else {
    // ➡️ Fallo: La función devolvió 0.
    cerr << " Falló la creación o el enlazado del historial médico.\n";
} return true;
}
bool modificarCita_BIN(const Cita& cModificada) {
    // 1. Validar que el ID de la cita es positivo
    if (cModificada.id <= 0) {
        cerr << " ID de cita inválido para modificación." << endl;
        return false;
    }
    
    
    fstream file("citas.bin", ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        cerr << " No se pudo abrir el archivo citas.bin para modificación." << endl;
        return false;
    }

   
    long pos = (long)(cModificada.id - 1) * sizeof(Cita);

    
    file.seekp(pos);

    // 5. Sobrescribir SOLO el registro completo del disco con los nuevos datos
    if (file.write(reinterpret_cast<const char*>(&cModificada), sizeof(Cita))) {
        file.close();
        // cout << "Registro de Cita ID " << cModificada.id << " modificado con éxito." << endl; // Opcional
        return true;
    } else {
        cerr << "Falló la escritura del registro de Cita ID " << cModificada.id << "." << endl;
        file.close();
        return false;
    }
}


bool cancelarCita_BIN() {
    int idCita = obtenerEntero("\nID de la Cita a cancelar: ");

    
    Cita cita = buscarCitaPorID_BIN(idCita); 

    
    if (cita.id == 0 || !cita.activa) {
        cerr << " Cita ID " << idCita << " no encontrada o ya está inactiva/cancelada." << endl;
        return false;
    }

    
    cita.activa = false; 
    strcpy(cita.estado, "CANCELADA");

    
    if (modificarCita_BIN(cita)) {
        cout << "Cita ID " << cita.id << " ha sido cancelada exitosamente." << endl;
        return true;
    } else {
        cerr << "Error al intentar guardar la cancelación de la Cita ID " << cita.id << "." << endl;
        return false;
    }
}



// Asumimos que las estructuras Cita y ArchivoHeader están definidas.

void listarCitasPorPaciente(int idPaciente) {
    const char* nombreArchivo = "citas.bin";
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo.is_open()) {
        cerr << " No se pudo abrir citas.bin para listado.\n";
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
        cerr << " No se pudo abrir citas.bin para listado.\n";
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
    // 1. Verificar nulo o vacío
    if (cedula == nullptr || strlen(cedula) == 0) return false;
    
    if (strlen(cedula) > 20) {
        cerr << "La cédula excede el límite de 20 caracteres." << endl;
        return false;
    }
    
    // Aquí podrías agregar validación de solo dígitos si es necesario.
    return true;
}


bool validarHora(const char* hora) {
    if (strlen(hora) != 5) {
        cerr << "El formato de hora debe ser exactamente HH:MM." << endl;
        return false;
    }
    
    if (hora[2] != ':') {
        cerr << "El formato de hora debe usar dos puntos (:) como separador." << endl;
        return false;
    }
    
    return true;
}

void cerrarSistema_BIN() {
    cout << "\n--- CERRANDO SISTEMA DE GESTIÓN ---\n";
    
    // Asume que guardarMetadatos() ahora lee la variable global 'hospital'.
    if (guardarMetadatos()) {
        cout << "✅ Estado global (Metadatos) guardado exitosamente en hospital.bin.\n";
    } else {
        cerr << "ADVERTENCIA: Fallo al guardar el estado global. Los IDs podrían perderse.\n";
    }

    // Ya no necesitas liberar memoria dinámica de arrays.
    cout << "Cierre de archivos y memoria completada.\n";
}


int main() {
    Hospital hospital;
    
    
    

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
        cout << "| 12. Eliminar paciente                |\n"; 
        cout << "| 13. Eliminar doctor                  |\n"; 
        cout << "| 0. Salir y Guardar                   |\n";
        cout << "+--------------------------------------+\n";
        opcion = obtenerEntero("Seleccione una opcion: ");

        
    } while (opcion != 0);

        switch (opcion) {
            case 1: { 
                Paciente p;
                cout << "Ingrese nombre: "; cin.getline(p.nombre, 50);
                cout << "Ingrese edad: "; cin >> p.edad; 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ingrese direccion: "; cin.getline(p.direccion, 100);
                cout << "Ingrese id: "; cin.ignore(p.id);
                if (agregarPaciente(p)) cout << "Registrado.\n";
                else cout << "Error al registrar.\n";
                break;
            }
            case 2: { 
                crearDoctor_BIN(); 
                break;
            }
            case 3: { 
                int idPaciente = obtenerEntero("ID del paciente: ");
                int idDoctor = obtenerEntero("ID del doctor: ");
                char fecha[11], hora[6];
                cout << "Fecha (DD/MM/AAAA): "; cin.getline(fecha, 11);
                cout << "Hora (HH:MM): "; cin.getline(hora, 6);
                crearCita_BIN();
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

                atenderCita_BIN(idCita);
                break;
            }
            case 5: { 
                int idPaciente = obtenerEntero("Ingrese el ID del paciente para ver historial: ");
                mostrarHistorial_BIN(idPaciente);
                break;
            }
            case 6: { 
                int idCancelar = obtenerEntero("Ingrese el ID de la cita que desea cancelar: ");
                cancelarCita_BIN(); 
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
                buscarDoctoresPorEspecialidad_BIN(especialidad); 
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
                eliminarPaciente_BIN(idPaciente); 
                break;
            }
            case 13: { 
                int idDoctor = obtenerEntero("ID del doctor a ELIMINAR logicamente: ");
                eliminarDoctor_BIN(idDoctor); 
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
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cin.get(); 
            
            system("cls || clear");
        }

    cerrarSistema_BIN();

    return 0;
}

