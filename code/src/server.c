#include <server.h>

int equalStrings(char * string_1, char * string_2, int size)
{
  int result = 1;
  for (int i = 0; i < size; i++)
  {
    if (string_1[i] != string_2[i])
    {
      return 0;
    }
  }
  return 1;
}

unsigned char * handleConf(int connfd)
{
  char buffer_method[4]; 
  long bytes_read_method;

  asm volatile
  (
    "syscall"
    : "=a"(bytes_read_method)
    : "a"(0), "D"(connfd), "S"(buffer_method), "d"(sizeof(buffer_method))
    : "rcx", "r11", "memory"
  );
  if (equalStrings(buffer_method, "POST", 4))
  {
    int content_size_integer;
    int found = 0;
    int i;
    char buffer[4096]; 
    long bytes_read;

    while (!found)
    {
      asm volatile
      (
        "syscall"
        : "=a"(bytes_read)
        : "a"(0), "D"(connfd), "S"(buffer), "d"(sizeof(buffer))
        : "rcx", "r11", "memory"
      );
      i = 0;
      while ((i < bytes_read) && (!found))
      {
        found = equalStrings(& buffer[i], "Content-Length", 14);
        i++;
      }
    }

    if (found)
    {
      int end_content_size;
      int found_length = 0;
      int index = i + 15;
      while (!found_length)
      {
        if (buffer[index] != '\r')
        {
          index++;
        }
        else
        {
          found_length = 1;
        }
      }
      end_content_size = index - i - 15;
      char cont_size[end_content_size];
      for (int j = 0; j < end_content_size; j++)
      {
        cont_size[j] = buffer[i + 15 + j];
      }
      content_size_integer = 0;
      for (int j = 0; j < end_content_size; j++)
      {
        content_size_integer = content_size_integer * 10 + (int) (cont_size[j] - '0');
      }
    }
    
    printInteger(content_size_integer);
    print("\n");

    int found_content = 0;

    i = 0;

    while ((i < bytes_read) && (!found_content))
    {
      found_content = equalStrings(& buffer[i], "\r\n\r\n", 4);
      i++;
    }

    printInteger(bytes_read);
    print("\n");

    printInteger(found_content);
    print("\n");

    while (!found_content)
    {
      asm volatile
      (
        "syscall"
        : "=a"(bytes_read)
        : "a"(0), "D"(connfd), "S"(buffer), "d"(sizeof(buffer))
        : "rcx", "r11", "memory"
      );
      i = 0;
      while ((i < bytes_read) && (!found_content))
      {
        found_content = equalStrings(& buffer[i], "\r\n\r\n", 4);
        i++;
      }
    }
    
    i--;
    printInteger(i);
    print("\n");
    if (buffer[i+3] == '\n'){
      print("YES\n");
    }
    print("\n");

    int first_chunk = bytes_read - i - 4;

    char * image = (char * ) malloc(content_size_integer);

    for (int index_image = i + 4; index_image < bytes_read; index_image++)
    {
      image[index_image - i - 4] = buffer[index_image];
    }

    char remain_buffer[content_size_integer - first_chunk];
    int new_bytes_read;
    int total_bytes_read = 0;
    int read_index = first_chunk;
    while (total_bytes_read < sizeof(remain_buffer))
    {
      asm volatile
      (
        "syscall"
        : "=a"(new_bytes_read)
        : "a"(0), "D"(connfd), "S"(remain_buffer), "d"(sizeof(remain_buffer))
        : "rcx", "r11", "memory"
      );
      total_bytes_read += new_bytes_read;
      for (int index_image = 0; index_image < new_bytes_read; index_image++)
      {
        image[read_index++] = remain_buffer[index_image];
      }
    }
    printInteger(total_bytes_read);
    print("\n");
    printInteger(sizeof(remain_buffer));
    print("\n");
    printInteger(new_bytes_read);
    print("\n");

    unsigned char * image_unsigned =  (unsigned char  * ) image;
    return image_unsigned;
  }
  else
  {
    return 0;
  }
}

int predictionSingle (LeNet5 * network, dataset2D * input_2d_1)
{
  dataset2D input_2d_2;
  dataset2D input_2d_3;
  dataset2D input_2d_4;
  dataset2D input_2d_5;
  dataset1D input_1d_1;
  dataset1D input_1d_2;
  dataset1D input_1d_3;
  dataset1D output;

  forwardLeNet5 (network, input_2d_1, & input_2d_2, & input_2d_3, & input_2d_4, & input_2d_5, & input_1d_1, & input_1d_2, & input_1d_3, & output);
  print ("th image, the prediction is:\n");
  for (int j = 0; j < 10; j++)
  {
    printDouble (output.samples[j]);
    print ("\n");
  }
  print ("\n\n\n");
  return argmax(output.samples, 10);
}

int displayInConsole (unsigned char * image_unsigned, LeNet5 * network)
{
  float * image = (float * ) malloc(640000);
  for (int i = 0; i < 400 * 400; i++)
  {
    image[i] = 255 - (float) image_unsigned[i * 4];
  }
  dataset2D image_dataset;
  image_dataset.channel_count = 1;
  image_dataset.column_count = 400;
  image_dataset.row_count = 400;
  image_dataset.sample_count = 1;
  image_dataset.samples = image;

  resizeImages (& image_dataset, 32, 32);
  displayImage (& image_dataset, 0, 0);
  print("\n");
  normalizeImages (& image_dataset);
  return predictionSingle (network, & image_dataset);
}

// Converts an integer to a string and returns the number of characters written.
// This simple version only handles non-negative integers.
int int_to_str(int num, char *buf) {
  int i = 0, j, len;
  char temp[16]; // temporary buffer (enough for most integers)

  if (num == 0) {
      buf[0] = '0';
      buf[1] = '\0';
      return 1;
  }

  // Extract digits in reverse order
  while (num > 0 && i < sizeof(temp)-1) {
      temp[i++] = (num % 10) + '0';
      num /= 10;
  }
  len = i;

  // Reverse the string into buf
  for (j = 0; j < i; j++) {
      buf[j] = temp[i - j - 1];
  }
  buf[len] = '\0';
  return len;
}

void start_server ()
{
  LeNet5 network;
  initializeLeNet5 (& network, 1);
  loadNetwork (& network, "/code/weights/final_weights.data");

  dataset2D Image;
  Image.channel_count = 1;
  Image.column_count = 28;
  Image.row_count = 28;
  Image.sample_count = 1;
  Image.samples = (float * ) malloc(28 * 28 * sizeof (float));

  int sockfd, connfd;

  struct sockaddr_in server_addr, client_addr;

  socklen_t client_len;

  long file_descriptor;
  long bytesRead;
  long return_value;
  struct stat file_stat;

  openFile ("/code/web/page.html", & file_descriptor, 0, 0);

  asm volatile(
    "syscall"
    : "=a"(return_value)
    : "a"(SYS_fstat), "D"(file_descriptor), "S"(& file_stat)
    : "rcx", "r11", "memory"
  );

  closeFile(file_descriptor);

  long file_size = file_stat.st_size;

  char * content = (char * ) malloc(file_size);

  openFile ("/code/web/page.html", & file_descriptor, 0, 0);

  readFile (file_size, content, & file_descriptor, & bytesRead);

  closeFile(file_descriptor);

  int index = 0;
  while (content[index] != '\0')
  {
    index++;
  }
  printInteger(index);
  print("\n");
  char response[] =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 3317\r\n"
      "Connection: close\r\n"
      "\r\n";

  asm volatile
  (
    "movq $41, %%rax;"    // SYS_socket
    "movl  $2, %%edi;"    // AF_INET
    "movl  $1, %%esi;"    // SOCK_STREAM
    "movl  $6, %%edx;"    // IPPROTO_TCP
    "syscall;"            // Trigger syscall
    : "=a" (sockfd)       // Output: store the return value in sockfd
    :                     // No input
    : "rdi", "rsi", "rdx" // Clobber registers
  );

  printInteger (sockfd);
  print ("\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = 0x901F;           
  for (int i = 0; i < 8; i++)
  {
    server_addr.sin_zero[i] = 0;
  }

  long ret;

  asm volatile
  (
    "syscall"
    : "=a"(ret)                 // return value
    : "a"(49),                  // syscall number for bind
      "D"(sockfd),              // first argument: socket file descriptor
      "S"(& server_addr),       // second argument: pointer to sockaddr struct
      "d"(sizeof (server_addr)) // third argument: size of sockaddr struct
    : "rcx", "r11", "memory"
  );

  printInteger (ret);
  print ("\n");

  asm volatile
  (
    "movq $50, %%rax;"    // SYS_listen
    "movl  %0, %%edi;"    // sockfd
    "movl  $5, %%esi;"    // backlog
    "syscall;"            // Trigger syscall
    :
    : "r" (sockfd)
    : "rax", "rdi", "esi"
  );

  print ("Server listening on port ");
  printInteger (PORT);
  print ("\n");
  while (1)
  {
    client_len = sizeof (client_addr);
    asm volatile
    (
      "syscall"
      : "=a"(connfd)
      : "a"(43), "D"(sockfd), "S"(&client_addr), "d"(&client_len)
      : "rcx", "r11", "memory"
    );
    unsigned char * image_unsigned = handleConf(connfd);

    //fitInDataset(& Image);
    //normalizeImages (& Image);
    //resizeImages (& Image, 32, 32);

    print ("Connection established\n");

    if (image_unsigned != 0)
    {
      print("Image received.\n");
      int prediction = displayInConsole(image_unsigned, & network);
      char http_headers[] = 
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 1\r\n"
      "Connection: close\r\n\r\n"; // 2x \r\n pour fin des headers
      
      // Conversion de la prédiction en char
      char prediction_char = prediction + '0';
      
      // Création du buffer de réponse complet
      char full_response[128];
      int header_length = 0;
      
      // Copie manuelle des headers
      while (http_headers[header_length] != '\0') {
          full_response[header_length] = http_headers[header_length];
          header_length++;
      }
      
      // Ajout de la prédiction
      full_response[header_length] = prediction_char;
      int total_length = header_length + 1; // Headers + 1 caractère
      asm volatile
      (
        "movq $1, %%rax;" // SYS_write
        "movl %0, %%edi;" // connfd
        "movq %1, %%rsi;" // response
        "movq %2, %%rdx;" // size of response
        "syscall;"        // Trigger syscall
        :
        : "r" (connfd), "r" (full_response), "r" ((long) total_length)
        : "rax", "rdi", "rsi", "rdx"
      );
    }
    else
    {
      asm volatile
      (
        "movq $1, %%rax;" // SYS_write
        "movl %0, %%edi;" // connfd
        "movq %1, %%rsi;" // response
        "movq %2, %%rdx;" // size of response
        "syscall;"        // Trigger syscall
        :
        : "r" (connfd), "r" (response), "r" (sizeof(response) - 1)
        : "rax", "rdi", "rsi", "rdx"
      );
      
      long bytes_to_write = file_size;  // Nombre total d’octets à envoyer
      long bytes_written = 0;           // Nombre d’octets déjà écrits
      long result = 0; 

      while (bytes_to_write > 0)
      {
        asm volatile
        (
          "syscall"
          : "=a"(result)       // Valeur de retour dans %rax
          : "a"(1),            // Numéro de l’appel système (1 pour SYS_write)
            "D"(connfd),           // Descripteur de fichier dans %rdi
            "S"(content + bytes_written),       // Buffer dans %rsi
            "d"(bytes_to_write)          // Taille dans %rdx
          : "rcx", "r11", "memory" // Registres modifiés par l’appel système
        );

        printInteger(connfd);
        print("\n");
        printInteger(result);
        print("\n");

        if (result <= 0) {  // Erreur ou connexion fermée
          print("Error writing content: ");
          printInteger(result);
          print("\n");
          break;
        }

        bytes_written += result;
        bytes_to_write -= result;

      }

    }
    asm volatile (
      "movq $48, %%rax;"   // SYS_shutdown
      "movl %0, %%edi;"    // File descriptor (connfd)
      "movl $1, %%esi;"    // SHUT_WR (stop writing)
      "syscall;"
      :
      : "r" (connfd)
      : "rax", "rdi", "rsi"
    );

    asm volatile
    (
      "movq $3, %%rax;" // SYS_close
      "movl %0, %%edi;" // connfd
      "syscall;"        // Trigger syscall
      :
      : "r" (connfd)
      : "rax", "rdi"
    );
    print ("Connection closed\n");
  }

  asm volatile
  (
    "movq  $60, %%rax;" // SYS_exit
    "xor %%rdi, %%rdi;" // Exit code 0
    "syscall;"          // Trigger syscall
    :
    : 
    : "rax", "rdi"
  );
}