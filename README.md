# Lab2:Link_layer

# Introducción

En la práctica, se establece una conexión serial entre dos dispositivos Bluepill con el propósito de enviar mensajes deliberadamente corruptos. El objetivo principal es evaluar la efectividad de dos sistemas de detección de errores: el sistema basado en el código CRC (Cyclic Redundancy Check) y el sistema de bit de paridad. Este enfoque permite analizar cómo ambos sistemas responden y detectan posibles corrupciones en los mensajes transmitidos, proporcionando así una evaluación integral de la robustez y fiabilidad de cada método de detección.

## Capa de enlace

La capa de enlace, ubicada como la segunda capa en el modelo TCP/IP, desempeña un papel esencial en la facilitación de la comunicación entre dispositivos en una red. Entre las funciones cruciales que ofrece se encuentran la encapsulación de tramas, asignación de direcciones físicas, control de acceso al medio, detección de errores, control de flujo y reensamblaje de tramas.

En el marco de la presente práctica, se han implementado diversas funciones inherentes a la capa de enlace, tales como la encapsulación y desencapsulación. No obstante, el énfasis principal ha sido dirigido hacia la detección de errores. Para abordar este aspecto, se han incorporado dos sistemas de detección de errores: el bit de paridad y el CRC (Cyclic Redundancy Check). Estas implementaciones específicas buscan fortalecer la integridad de la transmisión de datos, asegurando una comunicación confiable entre los dispositivos de la red.

# Implementación

## Bit de paridad

El bit de paridad, un método de detección de errores, se implementa mediante la adición de un bit adicional en cada conjunto de datos. Este bit se ajusta de manera que el número total de bits con valor "1" sea par o impar, según la configuración predefinida. Al recibir el mensaje, el receptor realiza una verificación de la paridad. Si concuerda con la configuración establecida, se asume que el mensaje ha sido recibido sin errores. En caso de discrepancia, se concluye que el mensaje ha experimentado corrupción durante la transmisión. Este sistema proporciona una forma eficaz y sencilla de detectar errores en la comunicación de datos.

No obstante, es crucial destacar que la robustez del sistema de bit de paridad tiene sus limitaciones. Si el mensaje presenta un único error, el sistema lo detecta eficientemente. Sin embargo, ante la presencia de dos o tres errores en el mensaje, existe la posibilidad de que no todos los errores sean identificados, debilitando su capacidad de detección en escenarios más complejos.

### Código

La implementación se inicia con la inclusión de bibliotecas y la definición de constantes como **`even`** (indicando par o impar), **`mode`** (modo de generación de errores) y **`message_num`** (estableciendo el número de mensajes a enviar). A continuación, se establece un espacio de nombres para la OLED y una variable de estado. Se declaran variables estáticas que almacenan los contadores de transmisión, errores y errores detectados.

Dos funciones principales son implementadas: **`setup()`**, que se ejecuta al iniciar el programa, y **`loop()`**, un bucle continuo que contiene la lógica del programa.

**`Setup():**` Configura la comunicación serial y la pantalla OLED. Inicializa el generador de números aleatorios y añade paridad al mensaje de prueba.

**`Loop():`** El bucle principal realiza transmisiones y recepciones, mostrando información en la pantalla OLED. En el modo de transmisión (cuando **`TX`** es verdadero), se envía el mensaje con errores de paridad al receptor. En el modo de recepción (cuando **`TX`** es falso), se recibe el mensaje, se verifica la paridad y se envía un ACK al transmisor.

Las funciones **`serial_tx(String &msg)`** y **`serial_rx(String &msg)`** contienen la lógica para la transmisión y recepción de mensajes, respectivamente.

**Funciones de Paridad:**

- **`addParity(String &msg)`**: Añade paridad a cada carácter del mensaje.
- **`getParity(char character)`**: Calcula y devuelve la paridad de un carácter.
- **`BitParity(String msg, int mode)`**: Introduce errores de paridad en el mensaje según el modo especificado.
- **`ParityCheck(String &msg)`**: Verifica la paridad del mensaje y cuenta los errores.
- **`clearParity(String msg)`**: Elimina la información de paridad de un mensaje.
- **`probabilidad()`**:Muestra en la pantalla OLED estadísticas sobre la probabilidad de error y la detección de errores. En ella, se presentan detalles como la cantidad de errores, la cantidad de mensajes recibidos y el porcentaje de errores detectados.

## CRC

El CRC (Cyclic Redundancy Check) representa un sistema de detección de errores más sólido y eficiente. En esta instancia específica, se implementó el CRC-8 utilizando el polinomio generador 0xD5. El proceso inicia con el transmisor realizando una división entre el mensaje y el polinomio generador. El residuo de esta operación se anexa al final de los datos, constituyendo el código CRC. Una vez calculado el CRC, los datos se transmiten.

En el receptor, se repite el proceso de división empleando el mismo polinomio generador. La integridad del mensaje se valida si el residuo de la división es igual a cero; de lo contrario, se infiere que el mensaje ha experimentado corrupción durante la transmisión.

Este enfoque con CRC proporciona una mayor robustez frente a la transmisión de una mayor cantidad de errores, ya que su capacidad de detección es más avanzada en comparación con métodos más simples como el bit de paridad.

### Código

Al igual que en el código previo, se inicia la implementación con la inclusión de librerías y la definición de constantes esenciales, tales como **`MSG_TEXT`**, **`TEST`**, **`TX`**, **`MODE`** y **`MESSAGE_NUM`**. Adicionalmente, se establece el espacio de nombres y se declaran variables estáticas como **`error_counter`**, **`tx_counter`** y **`detected_errors`**.

La función **`setup()`** inicializa la comunicación serial, configura la pantalla OLED, y establece el generador de números aleatorios. Asimismo, se añade paridad al mensaje invocando la función **`computeCRC`**.

Dentro del método **`loop()`**, se condiciona la lógica según el modo de operación. En caso de estar en modo de transmisión, se genera un mensaje con errores haciendo uso de la función **`burstGen`**. Mientras que en modo receptor, se verifica la integridad del mensaje calculando el código CRC.

Las funciones **`serial_tx(String &msg)`** y **`serial_rx(String &msg)`** siguen manteniendo la lógica para la transmisión y recepción de mensajes, respectivamente, proporcionando así una coherencia con el código anterior.

**Funciones CRC:**

- **`computeCRC(String& msg)`**: Realiza el cálculo del código CRC para el mensaje proporcionado.
- **`burstGen(String msg, int n)`**: Introduce errores en el mensaje simulando ráfagas de bits, permitiendo una evaluación de la robustez del sistema frente a dichos errores.
- **`crcCheck(String msg)`**: Verifica la integridad del mensaje mediante el cálculo del código CRC, permitiendo identificar posibles errores de transmisión.
- **`probabilidad()`**: Exhibe en la pantalla OLED estadísticas relevantes al término de la transmisión de mensajes, brindando información sobre la probabilidad de error y la eficacia de la detección de errores.

# Funcionamiento
